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

// Buffer sizes (in bytes) corresponding to BufferType
static const size_t BUFFER_SIZES[3] = {
    CHUNK_VOLUME * 8 * 5 * sizeof(f32),
    CHUNK_VOLUME * 36 * sizeof(u32),
    CHUNK_VOLUME * 96 * sizeof(u32)
};

struct Face {
    // index of face indices in 'indices' ChunkMeshBuffer    
    size_t indices_base;

    // position of face (used for sorting)
    vec3s position;

    // precomputed distance (used for sorting)
    f32 distance;
};

// Global buffer allocator
#define ALLOCATOR_ARRAY_CAPACITY 64
static struct {
    struct {
        void **array[ALLOCATOR_ARRAY_CAPACITY];
        size_t size;
    } buffers[BUFFER_TYPE_LAST + 1];
} allocator;

// appends data to the end of a ChunkMeshBuffer
#define CBAPPEND(_type, _buffer, _v) ((_type *) (_buffer)->data)[(_buffer)->index++] = (_v)

static void buffer_init(struct ChunkMeshBuffer *self, enum BufferType type) {
    memset(self, 0, sizeof(struct ChunkMeshBuffer));
    self->type = type;
}

static void buffer_destroy(struct ChunkMeshBuffer *self) {
    if (self->data != NULL) {
        free(self->data);
        self->data = NULL;
    }
}

static void buffer_alloc(struct ChunkMeshBuffer *self) {
    assert(self->data == NULL);
    self->capacity = BUFFER_SIZES[self->type];

    if (allocator.buffers[self->type].size == 0) {
        void *buffer = malloc(self->capacity);
        self->data = buffer;
    } else {
        self->data = allocator.buffers[self->type].array[--allocator.buffers[self->type].size];
    }
}

static void buffer_free(struct ChunkMeshBuffer *self) {
    if (allocator.buffers[self->type].size == ALLOCATOR_ARRAY_CAPACITY) {
        free(self->data);
        self->data = NULL;
    } else {
        allocator.buffers[self->type].array[allocator.buffers[self->type].size++] = self->data;
        self->data = NULL;
    }
}

struct ChunkMesh *chunkmesh_create(struct Chunk *chunk, bool depth_sort) {
    struct ChunkMesh *self = calloc(1, sizeof(struct ChunkMesh));
    self->flags.depth_sort = depth_sort;
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
    self->count = 0;
    self->index = 0;
}

static int depth_cmp(const struct Face *a, const struct Face *b) {
    return (int) -sign(a->distance - b->distance);
}

static void chunkmesh_depth_sort(struct ChunkMesh *self, vec3s center, bool upload) {
    struct ChunkMeshBuffer
        *faces = &self->buffers[FACES],
        *indices = &self->buffers[INDICES];

    // calulate face distances
    for (size_t i = 0; i < faces->count; i++) {
        struct Face *face = &((struct Face*) faces->data)[i];
        face->distance = glms_vec3_norm2(glms_vec3_sub(center, face->position));
    }

    // sort faces
    qsort(
        faces->data, faces->count, sizeof(struct Face),
        (int (*)(const void*, const void*)) depth_cmp);

    // reorder indices according to face ordering
    struct ChunkMeshBuffer t;
    buffer_init(&t, INDICES);
    buffer_alloc(&t);
    buffer_prepare(&t);

    for (size_t i = 0; i < faces->count; i++) {
        struct Face *face = &((struct Face*) faces->data)[i];
        memcpy(&((u32*) t.data)[i * 6], &((u32*) indices->data)[face->indices_base], 6 * sizeof(u32));
        face->indices_base = i * 6;
    }

    memcpy(indices->data, t.data, indices->count * sizeof(u32));
    buffer_free(&t);
    buffer_destroy(&t);

    if (upload) {
        vbo_buffer(self->ibo, self->buffers[INDICES].data, 0, (self->buffers[INDICES].count) * sizeof(u32));
    }
}

// MUST be called before meshing
static void chunkmesh_prepare(struct ChunkMesh *self) {
    self->vertex_count = 0;

    for (size_t i = 0; i <= BUFFER_TYPE_LAST; i++) {
        if (self->buffers[i].data == NULL) {
            buffer_alloc(&self->buffers[i]);
        }

        buffer_prepare(&self->buffers[i]);
    }
}

// MUST be called immediately after meshing (before rendering)
static void chunkmesh_finalize(struct ChunkMesh *self) {
    // flip each buffer
    for (size_t i = 0; i <= BUFFER_TYPE_LAST; i++) {
        struct ChunkMeshBuffer *buffer = &self->buffers[i];
        buffer->count = buffer->index;
        buffer->index = 0;
    }

    vbo_buffer(self->vbo, self->buffers[DATA].data, 0, (self->buffers[DATA].count) * sizeof(f32));
    
    if (self->flags.depth_sort) {
        chunkmesh_depth_sort(self, self->chunk->world->player.camera.position, false);
    }

    vbo_buffer(self->ibo, self->buffers[INDICES].data, 0, (self->buffers[INDICES].count) * sizeof(u32));

    buffer_free(&self->buffers[DATA]);

    if (!self->flags.persist) {
        buffer_free(&self->buffers[INDICES]);
        buffer_free(&self->buffers[FACES]);
    }
}

void chunkmesh_render(struct ChunkMesh *self) {
    if (self->buffers[INDICES].count == 0) {
        return;
    }

    shader_bind(state.shader);
    shader_uniform_camera(state.shader, state.world.player.camera);
    shader_uniform_mat4(state.shader, "m", glms_translate(glms_mat4_identity(), IVEC3S2V(self->chunk->position)));
    shader_uniform_texture2D(state.shader, "tex", state.block_atlas.atlas.texture, 0);

    const size_t vertex_size = 8 * sizeof(f32);
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, vertex_size, 0 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, vertex_size, 3 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 2, 3, GL_FLOAT, vertex_size, 5 * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, self->buffers[INDICES].count, GL_UNSIGNED_INT, NULL);
}

static inline void emit_sprite(
    struct ChunkMesh *mesh, vec3s position, vec2s uv_offset, vec2s uv_unit) {
    struct ChunkMeshBuffer
        *data = &mesh->buffers[DATA],
        *faces = &mesh->buffers[FACES],
        *indices = &mesh->buffers[INDICES];

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
        for (size_t j = 0; j < 3; j++) {
            CBAPPEND(f32, data, 1.0f);
        } 
    }

    // emit indices
    for (size_t i = 0; i < 12; i++) {
        CBAPPEND(u32, indices, mesh->vertex_count + SPRITE_INDICES[i]);
    }

    // emitte 8 vertices
    mesh->vertex_count += 8; 
}

static inline void emit_face(
    struct ChunkMesh *mesh, vec3s position, enum Direction direction, 
    vec2s uv_offset, vec2s uv_unit, bool transparent, bool shorten_y) {
    struct ChunkMeshBuffer
        *data = &mesh->buffers[DATA],
        *faces = &mesh->buffers[FACES],
        *indices = &mesh->buffers[INDICES];

    // add this face into the face buffer if it's transparent
    if (transparent) {
        struct Face face = {
            .indices_base = indices->index,
            .position = position
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

        // TODO: real lighting
        // vary color according to face direction
        f32 color;
        if (transparent) {
            color = 1.0f;
        } else {
            switch (direction) {
                case UP:
                    color = 1.0f;
                    break;
                case NORTH:
                case SOUTH:
                    color = 0.86f;
                    break;
                case EAST:
                case WEST:
                    color = 0.8f;
                    break;
                case DOWN:
                    color = 0.6f;
                    break;
            }
        }

        for (size_t j = 0; j < 3; j++) {
            CBAPPEND(f32, data, color);
        } 
    }

    // emit indices
    for (size_t i = 0; i < 6; i++) {
        CBAPPEND(u32, indices, mesh->vertex_count + FACE_INDICES[i]);
    }

    // emitted 4 more vertices, bump the vertex count
    mesh->vertex_count += 4;
}

void chunk_mesh(struct Chunk *self, enum ChunkMeshPass pass) {
    if (pass == FULL) {
        chunkmesh_prepare(self->meshes.base);
    }

    chunkmesh_prepare(self->meshes.transparent);

    chunk_foreach(pos) {
        vec3s fpos = IVEC3S2V(pos);
        ivec3s wpos = glms_ivec3_add(pos, self->position);

        u32 data = self->data[chunk_pos_to_index(pos)];
        if(data != 0) {
            struct Block block = BLOCKS[data], neighbor_block;
            bool transparent = block.is_transparent(self->world, wpos);
            
            if (block.is_sprite()) {
                emit_sprite(
                    self->meshes.transparent, fpos,
                    atlas_offset(state.block_atlas.atlas, BLOCKS[data].get_texture_location(self->world, wpos, NORTH)),
                    state.block_atlas.atlas.sprite_unit);
            } else {
                bool shorten_y = false;

                if (block.is_liquid()) {
                    ivec3s up = (ivec3s) {{ pos.x, pos.y + 1, pos.z }};

                    if (chunk_in_bounds(up)) {
                        shorten_y = !BLOCKS[self->data[chunk_pos_to_index(up)]].is_liquid();
                    } else {
                        shorten_y = !BLOCKS[world_get_data(self->world, glms_ivec3_add(up, self->position))].is_liquid();
                    }
                }

                for (enum Direction d = 0; d < 6; d++) {
                    ivec3s dv = DIR2IVEC3S(d);
                    ivec3s neighbor = glms_ivec3_add(pos, dv), wneighbor = glms_ivec3_add(wpos, dv);

                    if (chunk_in_bounds(neighbor)) {
                        neighbor_block = BLOCKS[self->data[chunk_pos_to_index(neighbor)]];
                    } else {
                        neighbor_block = BLOCKS[world_get_data(self->world, wneighbor)];
                    }

                    bool neighbor_transparent = neighbor_block.is_transparent(self->world, wneighbor);
                    if (neighbor_transparent && (
                        (pass == FULL && !transparent) ||
                        (transparent && neighbor_block.id != block.id))) {

                        emit_face(
                            transparent ? self->meshes.transparent : self->meshes.base, fpos, d,
                            atlas_offset(state.block_atlas.atlas, block.get_texture_location(self->world, wpos, d)),
                            state.block_atlas.atlas.sprite_unit, transparent, shorten_y);
                    }
                }
            }
        }
    }

    if (pass == FULL) {
        chunkmesh_finalize(self->meshes.base);
    }

    chunkmesh_finalize(self->meshes.transparent);
}

void chunk_depth_sort(struct Chunk *self) {
    if (self->meshes.transparent->flags.persist
        && self->meshes.transparent->buffers[INDICES].data != NULL
        && self->meshes.transparent->buffers[FACES].data != NULL) {
        chunkmesh_depth_sort(self->meshes.transparent, self->world->player.camera.position, true);
    } else {
        chunk_mesh(self, TRANSPARENCY);
    }
}

void chunkmesh_set_persist(struct ChunkMesh *self, bool persist) {
    if (self->flags.persist == persist) {
        return;
    }

    self->flags.persist = persist;

    if (!self->flags.persist) {
        buffer_free(&self->buffers[INDICES]);
        buffer_free(&self->buffers[FACES]);
    } else {
        buffer_alloc(&self->buffers[INDICES]);
        buffer_alloc(&self->buffers[FACES]);
    }
}