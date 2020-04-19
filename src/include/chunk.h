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

struct Face {
    size_t indices_base;
    vec3s position;
    f32 distance;
};

struct MeshBuffer {
    void *data;
    size_t index, count, capacity;
};

struct Mesh {
    struct Chunk *chunk;

    struct MeshBuffer data, faces, indices;
    size_t vertex_count;

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

    // if true, this chunk will do a transparency depth sort next frame
    bool depth_sort;
    
    struct {
        struct Mesh base, transparent;
    } meshes;
};


bool chunk_in_bounds(ivec3s pos);

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset);
void chunk_destroy(struct Chunk *self);
void chunk_set_data(struct Chunk *self, ivec3s pos, u32 data);
u32 chunk_get_data(struct Chunk *self, ivec3s pos);
void chunk_render(struct Chunk *self);
void chunk_render_transparent(struct Chunk *self);
void chunk_update(struct Chunk *self);
void chunk_tick(struct Chunk *self);

#endif