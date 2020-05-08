#include "../state.h"
#include "chunk.h"

// Buffer sizes (in bytes) corresponding to BufferType
static const size_t BUFFER_SIZES[3] = {
    (CHUNK_VOLUME / 2) * 6 * 6 * 8 * sizeof(f32),
    (CHUNK_VOLUME / 2) * 6 * 6 * sizeof(u32),
    (CHUNK_VOLUME / 2) * 6 * sizeof(struct Face)
};

// gets data at the specified position in the chunk if the position is inside of
// the chunk, otherwise gets data from the world
#define CHUNK_WORLD_GET_DATA(chunk, pos) ({\
    ivec3s p = (pos);\
    struct Chunk *c = (chunk);\
    chunk_in_bounds(p) ?\
        chunk_get_data(c, p) :\
        world_get_data(c->world, glms_ivec3_add(c->position, p));})

static void buffer_init(struct ChunkMeshBuffer *self, enum BufferType type) {
    memset(self, 0, sizeof(struct ChunkMeshBuffer));
    self->type = type;
    self->capacity = BUFFER_SIZES[type];
}

static void buffer_destroy(struct ChunkMeshBuffer *self) {
    if (self->data != NULL) {
        free(self->data);
    }
}

struct ChunkMesh *chunkmesh_create(struct Chunk *chunk) {
    struct ChunkMesh *self = calloc(1, sizeof(struct ChunkMesh));
    self->chunk = chunk;
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, false);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);

    for (size_t i = 0; i <= BUFFER_TYPE_LAST; i++) {
        buffer_init(&self->buffers[i], i);
    }

    return self;
}

void chunkmesh_destroy(struct ChunkMesh *self) {
    vao_destroy(self->vao);
    vbo_destroy(self->vbo);
    vbo_destroy(self->ibo);

    for (size_t i = 0; i <= BUFFER_TYPE_LAST; i++) {
        buffer_destroy(&self->buffers[i]);
    }
}

static void buffer_prepare(struct ChunkMeshBuffer *self) {
    if (self->data == NULL) {
        self->data = malloc(self->capacity);
    }

    self->count = 0;
    self->index = 0;
    self->elements = 0;
}

// MUST be called before meshing
void chunkmesh_prepare(struct ChunkMesh *self) {
    self->vertex_count = 0;

    for (size_t i = 0; i <= BUFFER_TYPE_LAST; i++) {
        buffer_prepare(&self->buffers[i]);
    }
}

static void chunkmesh_flip_buffers(struct ChunkMesh *self) {
    // self->buffers[*].elements is already set
    self->buffers[DATA].count = self->buffers[DATA].index;
    self->buffers[INDICES].count = self->buffers[INDICES].index;
    self->buffers[FACES].count = self->buffers[FACES].index;
}

// MUST be called immediately after meshing (before rendering)
static void chunkmesh_finalize_data(struct ChunkMesh *self) {
    assert(self->buffers[DATA].data != NULL);
    vbo_buffer(
        self->vbo, self->buffers[DATA].data, 0,
        self->buffers[DATA].count);

    free(self->buffers[DATA].data);
    self->buffers[DATA].data = NULL;
}

// MUST be called immediately after meshing AND sorting (before rendering)
static void chunkmesh_finalize_indices(struct ChunkMesh *self) {
    assert(self->buffers[INDICES].data != NULL);
    vbo_buffer(
        self->ibo, self->buffers[INDICES].data, 0,
        self->buffers[INDICES].count);

    if (!self->flags.persist) {
        free(self->buffers[INDICES].data);
        self->buffers[INDICES].data = NULL;

        free(self->buffers[FACES].data);
        self->buffers[FACES].data = NULL;
    }
}

// type of sort to perform
// a SORT_FULL sort will moved non-transparent indices to the end of the buffer
// a SORT_PARTIAL sort assumes that the indices have had a SORT_FULL run
// previously and only reorders indices in the transparent portion of the buffer 
enum SortKind {
    SORT_FULL, SORT_PARTIAL
};

static int depth_cmp(const struct Face *a, const struct Face *b) {
    return (int) -sign(a->distance - b->distance);
}

static void chunkmesh_sort(struct ChunkMesh *self, enum SortKind kind) {
    vec3s center = ((struct PositionComponent *) ecs_get(self->chunk->world->entity_view, C_POSITION))->position;

    struct ChunkMeshBuffer
        *faces = &self->buffers[FACES],
        *indices = &self->buffers[INDICES];

    assert(faces->data != NULL);
    assert(indices->data != NULL);

    // create a temporary indices buffer
    struct ChunkMeshBuffer t_indices = {0};
    t_indices.capacity = BUFFER_SIZES[INDICES];
    t_indices.data = malloc(t_indices.capacity);

    memcpy(t_indices.data, indices->data, indices->count);

    // calulate face distances
    for (size_t i = 0; i < faces->elements; i++) {
        struct Face *face = &((struct Face*) faces->data)[i];
        face->distance = glms_vec3_norm2(glms_vec3_sub(center, face->position));
    }

    // sort faces
    qsort(
        faces->data, faces->elements, sizeof(struct Face),
        (int (*)(const void*, const void*)) depth_cmp);

    Bitmap moved = kind == SORT_FULL ? bitmap_calloc(indices->elements / 6) : NULL;

    // iterate over sorted faces: move them (in order) into the front of the index buffer
    for (size_t i = 0; i < faces->elements; i++) {
        struct Face *face = &((struct Face*) faces->data)[i];
        if (face->indices_base != i * 6) {
            memcpy(
                &((u32*) indices->data)[i * 6],
                &((u32*) t_indices.data)[face->indices_base],
                6 * sizeof(u32)
            );
        }

        if (moved != NULL) {
            bitmap_set(moved, face->indices_base / 6);
        }

        face->indices_base = i * 6;
    }

    // only move non-transparent indices around on a full sort
    if (kind == SORT_FULL) {
        self->indices.transparent.offset = 0;
        self->indices.transparent.count = faces->elements * 6;

        self->indices.base.offset = self->indices.transparent.count;
        self->indices.base.count = 0;

        // iterate over ALL faces, move those that weren't originally moved to the end of the index buffer
        for (size_t i = 0; i < (indices->elements / 6); i++) {
            if (!bitmap_get(moved, i)) {
                memcpy(
                    &((u32*) indices->data)[self->indices.base.offset + self->indices.base.count],
                    &((u32*) t_indices.data)[i * 6],
                    6 * sizeof(u32)
                );
                self->indices.base.count += 6;
            }
        }
    }

    if (moved != NULL) {
        free(moved);
    }

    free(t_indices.data);
}

static void chunkmesh_mesh(struct ChunkMesh *self) {
    struct Chunk *chunk = self->chunk;
    struct BlockAtlas *block_atlas = &state.renderer.block_atlas;

    chunkmesh_prepare(self);

    chunk_foreach(pos) {
        struct BlockMeshParams params;
        params.position = IVEC3S2V(pos);
        params.data = chunk->data[chunk_pos_to_index(pos)];        

        enum BlockId block_id = chunk_data_to_block(params.data);
        ivec3s pos_w = glms_ivec3_add(pos, chunk->position);

        if(block_id != AIR) {
            params.block = &BLOCKS[block_id];
            
            switch (params.block->mesh_type) {
                case BLOCKMESH_SPRITE:
                    atlas_get(
                        block_atlas->atlas,
                        params.block->get_texture_location(chunk->world, pos_w, 0),
                        &params.uv_min, &params.uv_max);
                    blockmesh_sprite(self, params);
                    break;
                case BLOCKMESH_CUSTOM:
                case BLOCKMESH_DEFAULT:
                case BLOCKMESH_LIQUID:
                    params.offset = (vec3s) {{ 0, 0, 0 }};
                    params.size = (vec3s) {{ 1, 1, 1 }};

                    for (enum Direction d = 0; d < 6; d++) {
                        ivec3s d_v = DIR2IVEC3S(d);
                        ivec3s n = glms_ivec3_add(pos, d_v);
                        
                        params.data_n = CHUNK_WORLD_GET_DATA(chunk, n);
                        params.block_n = &BLOCKS[chunk_data_to_block(params.data_n)];

                        if (params.block->mesh_type == BLOCKMESH_CUSTOM ||
                            (params.block_n->transparent && !params.block->transparent) ||
                            (params.block->transparent &&
                                params.block_n->transparent &&
                                params.block_n->id != params.block->id)) {
                            params.direction = d;

                            // get base UV coordinates
                            atlas_get(
                                block_atlas->atlas,
                                params.block->get_texture_location(chunk->world, pos_w, d),
                                &params.uv_min, &params.uv_max);

                            // get custom mesh parameters for this face
                            if (params.block->mesh_type == BLOCKMESH_CUSTOM) {
                                ivec2s uv_offset_px, uv_size_px;
                                params.block->get_mesh_information(
                                    chunk->world, pos, params.direction,
                                    &params.offset, &params.size,
                                    &uv_offset_px, &uv_size_px);

                                // offset UV coordinates
                                vec2s uv_offset = glms_vec2_mul(
                                    IVEC2S2V(uv_offset_px),
                                    block_atlas->atlas.pixel_unit),
                                    uv_size = glms_vec2_mul(
                                    IVEC2S2V(uv_size_px),
                                    block_atlas->atlas.pixel_unit);

                                params.uv_min = glms_vec2_add(params.uv_min, uv_offset);
                                params.uv_max = glms_vec2_add(params.uv_min, uv_size);
                            }

                            blockmesh_face(self, params);
                        }
                    }
                    break;
            }
        }
    }

    chunkmesh_flip_buffers(self);
    chunkmesh_sort(self, SORT_FULL);
    chunkmesh_finalize_data(self);
    chunkmesh_finalize_indices(self);
}

void chunkmesh_prepare_render(struct ChunkMesh *self) {
    if (self->chunk->world->throttles.mesh.count <
        self->chunk->world->throttles.mesh.max) {
        if (self->flags.dirty) {
            chunkmesh_mesh(self);
            self->flags.dirty = false;
            self->flags.depth_sort = false;
            self->chunk->world->throttles.mesh.count++;
        } else if (self->flags.depth_sort) {
            if (self->flags.persist &&
                self->buffers[INDICES].data != NULL &&
                self->buffers[FACES].data != NULL) {
                chunkmesh_sort(self, SORT_PARTIAL);
                chunkmesh_finalize_indices(self);
            } else {
                chunkmesh_mesh(self);
            }
            self->flags.depth_sort = false;
            self->chunk->world->throttles.mesh.count++;
        }
    }
}

void chunkmesh_render(struct ChunkMesh *self, enum ChunkMeshPart part) {
    // don't attempt to render empty meshes
    if ((part == TRANSPARENT && self->indices.transparent.count == 0) ||
        (part == BASE && self->indices.base.count == 0)) {
        return;
    }

    shader_uniform_mat4(
        state.renderer.shaders[SHADER_CHUNK], "m",
        glms_translate(glms_mat4_identity(), IVEC3S2V(self->chunk->position)));

    const size_t vertex_size = (5 * sizeof(f32)) + (1 * sizeof(u32));
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, vertex_size, 0 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, vertex_size, 3 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 2, 1, GL_UNSIGNED_INT, vertex_size, 5 * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);

    size_t offset, count;
    if (part == TRANSPARENT) {
        offset = self->indices.transparent.offset;
        count = self->indices.transparent.count;
    } else {
        offset = self->indices.base.offset;
        count = self->indices.base.count;
    }

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void *) (offset * sizeof(u32)));
}

void chunkmesh_set_persist(struct ChunkMesh *self, bool persist) {
    if (self->flags.persist == persist) {
        return;
    }

    self->flags.persist = persist;

    if (!self->flags.persist) {
        free(self->buffers[INDICES].data);
        free(self->buffers[FACES].data);
        
        self->buffers[INDICES].data = NULL;
        self->buffers[FACES].data = NULL;
    }
}