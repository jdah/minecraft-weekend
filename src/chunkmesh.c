#include "include/chunk.h"
#include "include/state.h"

static const u32 FACE_INDICES[] = {1, 0, 3, 1, 3, 2};
static const u32 UNIQUE_INDICES[] = {1, 0, 5, 2};
static const u32 CUBE_INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};

static const f32 FACE_CENTERS[] = {
    0.5f, 0.5f, 0.0f, // north (-z)
    0.5f, 0.5f, 1.0f, // south (+z)
    1.0f, 0.5f, 0.5f, // east (+x)
    0.0f, 0.5f, 0.5f, // west (-x)
    0.5f, 1.0f, 0.5f, // top (+y)
    0.5f, 0.0f, 0.5f, // bottom (-y)
};

static const u32 SPRITE_INDICES[] = {
    3, 0, 5, 3, 5, 6,
    4, 2, 1, 4, 2, 7
};

static const f32 CUBE_VERTICES[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,

    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
};

static const f32 CUBE_UVS[] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1
};

struct Face {
    // index of face indices in 'indices' ChunkMeshBuffer    
    size_t indices_base;

    // position of face (used for sorting)
    vec3s position;

    // precomputed distance (used for sorting)
    f32 distance;
};

// Buffer sizes (in bytes) corresponding to BufferType
static const size_t BUFFER_SIZES[3] = {
    (CHUNK_VOLUME / 2) * 6 * 6 * 8 * sizeof(f32),
    (CHUNK_VOLUME / 2) * 6 * 6 * sizeof(u32),
    (CHUNK_VOLUME / 2) * 6 * sizeof(struct Face)
};

// appends data to the end of a ChunkMeshBuffer
#define CBAPPEND(_type, _buffer, _v) ((_type *) (_buffer)->data)[(_buffer)->index++] = (_v)

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
}

// MUST be called before meshing
void chunkmesh_prepare(struct ChunkMesh *self) {
    self->vertex_count = 0;

    for (size_t i = 0; i <= BUFFER_TYPE_LAST; i++) {
        buffer_prepare(&self->buffers[i]);
    }
}

static void chunkmesh_flip_buffers(struct ChunkMesh *self) {
    self->buffers[DATA].count = self->buffers[DATA].index;
    self->buffers[INDICES].count = self->buffers[INDICES].index;
    self->buffers[FACES].count = self->buffers[FACES].index;
}

// MUST be called immediately after meshing (before rendering)
static void chunkmesh_finalize_data(struct ChunkMesh *self) {
    assert(self->buffers[DATA].data != NULL);
    vbo_buffer(
        self->vbo, self->buffers[DATA].data, 0,
        self->buffers[DATA].count * sizeof(f32));

    free(self->buffers[DATA].data);
    self->buffers[DATA].data = NULL;
}

// MUST be called immediately after meshing AND sorting (before rendering)
static void chunkmesh_finalize_indices(struct ChunkMesh *self) {
    assert(self->buffers[INDICES].data != NULL);
    vbo_buffer(
        self->ibo, self->buffers[INDICES].data, 0,
        self->buffers[INDICES].count * sizeof(u32));

    if (!self->flags.persist) {
        free(self->buffers[INDICES].data);
        self->buffers[INDICES].data = NULL;

        free(self->buffers[FACES].data);
        self->buffers[FACES].data = NULL;
    }
}

static inline void emit_sprite(
    struct ChunkMesh *self, vec3s position, vec2s uv_offset, vec2s uv_unit) {
    struct ChunkMeshBuffer
        *data = &self->buffers[DATA],
        *faces = &self->buffers[FACES],
        *indices = &self->buffers[INDICES];

    // emitting 2 transparent faces
    for (size_t i = 0; i < 2; i++) {
        struct Face face = {
            .indices_base = indices->index + (i * 6),
            .position = position
        };

        memcpy(((struct Face *) faces->data) + faces->index, &face, sizeof(struct Face));
        faces->index++;
    }

    // emit vertices
    for (size_t i = 0; i < 8;  i++) {
        const f32 *vertex = &CUBE_VERTICES[i * 3];
        CBAPPEND(f32, data, position.x + vertex[0]);
        CBAPPEND(f32, data, position.y + vertex[1]);
        CBAPPEND(f32, data, position.z + vertex[2]);
        CBAPPEND(f32, data, uv_offset.x + (uv_unit.x * CUBE_UVS[((i % 4) * 2) + 0]));
        CBAPPEND(f32, data, uv_offset.y + (uv_unit.y * CUBE_UVS[((i % 4) * 2) + 1]));

        // color
        // TODO: fix this
        CBAPPEND(f32, data, 0.0f);
    }

    // emit indices
    for (size_t i = 0; i < 12; i++) {
        CBAPPEND(u32, indices, self->vertex_count + SPRITE_INDICES[i]);
    }

    // emitted 8 vertices
    self->vertex_count += 8; 
}

static inline void emit_face(
    struct ChunkMesh *self, u64 block_data, u64 neighbor_data,
    vec3s position, enum Direction direction, 
    vec2s uv_offset, vec2s uv_unit, bool transparent, bool shorten_y) {
    struct ChunkMeshBuffer
        *data = &self->buffers[DATA],
        *faces = &self->buffers[FACES],
        *indices = &self->buffers[INDICES];
    
    // add this face into the face buffer if it's transparent
    if (transparent) {
        struct Face face = {
            .indices_base = indices->index,
            .position = glms_vec3_add((vec3s) {{
                FACE_CENTERS[(direction * 3) + 0],
                FACE_CENTERS[(direction * 3) + 1],
                FACE_CENTERS[(direction * 3) + 2]
            }}, position)
        };

        memcpy(((struct Face *) faces->data) + faces->index, &face, sizeof(struct Face));
        faces->index++;
    }

    // emit vertices
    for (size_t i = 0; i < 4;  i++) {
        const f32 *vertex = &CUBE_VERTICES[CUBE_INDICES[(direction * 6) + UNIQUE_INDICES[i]] * 3];
        CBAPPEND(f32, data, position.x + vertex[0]);
        CBAPPEND(f32, data, position.y + ((shorten_y ? 0.9f : 1.0f) * vertex[1]));
        CBAPPEND(f32, data, position.z + vertex[2]);
        CBAPPEND(f32, data, uv_offset.x + (uv_unit.x * CUBE_UVS[(i * 2) + 0]));
        CBAPPEND(f32, data, uv_offset.y + (uv_unit.y * CUBE_UVS[(i * 2) + 1]));

        // TODO: make buffers more robust, this is a hack
        // use lighting data of this face's NEIGHBOR to light it
        u32 light = chunk_data_to_all_light(neighbor_data);
        light |= ((u32) direction) << 20;
        CBAPPEND(f32, data, *((f32 *) (&light)));
    }

    // emit indices
    for (size_t i = 0; i < 6; i++) {
        CBAPPEND(u32, indices, self->vertex_count + FACE_INDICES[i]);
    }

    // emitted 4 more vertices, bump the vertex count
    self->vertex_count += 4;
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
    // TODO: there should be a better way of getting the center
    vec3s center = self->chunk->world->player.camera.position;

    struct ChunkMeshBuffer
        *faces = &self->buffers[FACES],
        *indices = &self->buffers[INDICES];

    assert(faces->data != NULL);
    assert(indices->data != NULL);

    // create a temporary indices buffer
    struct ChunkMeshBuffer t_indices;
    t_indices.capacity = BUFFER_SIZES[INDICES];
    t_indices.data = malloc(t_indices.capacity);

    memcpy(t_indices.data, indices->data, indices->count * sizeof(u32));

    // calulate face distances
    for (size_t i = 0; i < faces->count; i++) {
        struct Face *face = &((struct Face*) faces->data)[i];
        face->distance = glms_vec3_norm2(glms_vec3_sub(center, face->position));
    }

    // sort faces
    qsort(
        faces->data, faces->count, sizeof(struct Face),
        (int (*)(const void*, const void*)) depth_cmp);

    Bitmap moved = kind == SORT_FULL ? bitmap_calloc(indices->count / 6) : NULL;

    // iterate over sorted faces: move them (in order) into the front of the index buffer
    for (size_t i = 0; i < faces->count; i++) {
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
        self->indices.transparent.count = faces->count * 6;

        self->indices.base.offset = self->indices.transparent.count;
        self->indices.base.count = 0;

        // iterate over ALL faces, move those that weren't originally moved to the end of the index buffer
        for (size_t i = 0; i < (indices->count / 6); i++) {
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

    chunkmesh_prepare(self);

    chunk_foreach(pos) {
        vec3s pos_f = IVEC3S2V(pos);
        ivec3s pos_world = glms_ivec3_add(pos, chunk->position);

        u64 data = chunk->data[chunk_pos_to_index(pos)];
        enum BlockId block_id = chunk_data_to_block(data);

        if(block_id != AIR) {
            struct Block block = BLOCKS[block_id];
            bool transparent = block.is_transparent(chunk->world, pos_world);
            
            if (block.is_sprite()) {
                // emit_sprite(
                //     self, fpos,
                //     atlas_offset(
                //         state.renderer.block_atlas.atlas,
                //         block.get_texture_location(chunk->world, wpos, NORTH)),
                //     state.renderer.block_atlas.atlas.sprite_unit);
            } else {
                bool shorten_y = false;

                if (block.is_liquid()) {
                    shorten_y = !BLOCKS[
                        chunk_data_to_block(
                            CHUNK_WORLD_GET_DATA(
                                chunk,
                                ((ivec3s) {{ pos.x, pos.y + 1, pos.z }})))                                
                        ].is_liquid();
                }

                for (enum Direction d = 0; d < 6; d++) {
                    ivec3s dv = DIR2IVEC3S(d);
                    ivec3s n = glms_ivec3_add(pos, dv), n_world = glms_ivec3_add(pos_world, dv);
                    u64 neighbor_data = CHUNK_WORLD_GET_DATA(chunk, n);
                    struct Block neighbor_block = BLOCKS[chunk_data_to_block(neighbor_data)]; 

                    bool neighbor_transparent = neighbor_block.is_transparent(chunk->world, n_world);
                    if ((neighbor_transparent && !transparent) || (transparent && neighbor_block.id != block.id)) {
                        emit_face(
                            self, data, neighbor_data, pos_f, d,
                            atlas_offset(
                                state.renderer.block_atlas.atlas,
                                block.get_texture_location(chunk->world, pos_world, d)),
                            state.renderer.block_atlas.atlas.sprite_unit,
                            transparent, shorten_y);
                    }
                }
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