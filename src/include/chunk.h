#ifndef CHUNK_H
#define CHUNK_H

#include "util.h"
#include "gfx.h"
#include "vbo.h"
#include "vao.h"
#include "world.h"

#define CHUNK_SIZE ((ivec3s) {{ 16, 256, 16 }})
#define CHUNK_SIZE_F ((vec3s) {{ 16, 256, 16 }})

#define CHUNK_VOLUME (CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z)

struct MeshBuffer {
    // current number of vertices buffered and current buffer indices
    size_t vertex_count, data_index, indices_index;

    // intermediate buffers to write to while rendering
    f32 *data_buffer;
    u16 *index_buffer;

    // offset and count for rendering
    size_t indices_offset, indices_count;
};

struct Mesh {
    struct Chunk *chunk;

    // 7 separate buffers: 1 base (normal) and then 6 for each transparency direction
    struct MeshBuffer buffers[7];

    // buffer objects
    struct VAO vao;
    struct VBO vbo, ibo;
};

struct Chunk {
    struct World *world;

    // offset (in chunks) and position (in blocks)
    ivec3s offset, position;

    // block data
    u32 *data;

    // if true, this chunk will re-mesh next frame
    bool dirty;
    struct Mesh mesh;
};

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset);
void chunk_destroy(struct Chunk *self);
void chunk_set_data(struct Chunk *self, ivec3s pos, u32 data);
u32 chunk_get_data(struct Chunk *self, ivec3s pos);
void chunk_render(struct Chunk *self);
void chunk_update(struct Chunk *self);
void chunk_tick(struct Chunk *self);

#endif