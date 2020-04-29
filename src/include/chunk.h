#ifndef CHUNK_H
#define CHUNK_H

#include "util.h"
#include "gfx.h"
#include "vbo.h"
#include "vao.h"
#include "world.h"

#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32

#define CHUNK_SIZE ((ivec3s) {{ CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z }})
#define CHUNK_SIZE_F ((vec3s) {{ CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z }})

#define CHUNK_VOLUME (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

#define chunk_foreach(_pname)\
    ivec3s _pname = GLMS_IVEC3_ZERO_INIT;\
    for (s32 x = 0; x < CHUNK_SIZE.x; x++)\
        for (s32 y = 0; y < CHUNK_SIZE.y; y++)\
            for (s32 z = 0;\
                z < CHUNK_SIZE.z &&\
                ((_pname.x = x) != INT32_MAX) &&\
                ((_pname.y = y) != INT32_MAX) &&\
                ((_pname.z = z) != INT32_MAX);\
                z++)

#define chunk_pos_to_index(p) (p.x * CHUNK_SIZE.y * CHUNK_SIZE.z + p.y * CHUNK_SIZE.z + p.z)

struct Chunk {
    struct World *world;

    // offset (in chunks) and position (in blocks)
    ivec3s offset, position;

    // block data
    u32 *data;

    // number of blocks
    size_t count;

    struct {
        // if true, this chunk will re-mesh next frame
        bool dirty: 1;

        // if true, this chunk will do a transparency depth sort next frame
        bool depth_sort: 1;

        // if true, this chunk contains no blocks
        bool empty: 1;
    } flags;
    
    struct {
        struct ChunkMesh *base, *transparent;
    } meshes;
};

bool chunk_in_bounds(ivec3s pos);

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset);
void chunk_destroy(struct Chunk *self);
void chunk_set_data(struct Chunk *self, ivec3s pos, u32 data);
u32 chunk_get_data(struct Chunk *self, ivec3s pos);
void chunk_render(struct Chunk *self);
void chunk_update(struct Chunk *self);
void chunk_tick(struct Chunk *self);

// chunkmesh.c
#define BUFFER_TYPE_LAST FACES
enum BufferType {
    DATA = 0, INDICES, FACES
};

struct ChunkMeshBuffer {
    enum BufferType type;

    // data store for this buffer, NULL if not currently allocated
    void *data;

    // capacity (in bytes) of *data
    size_t capacity;
    
    // CURRENT index into *data, used when building mesh
    size_t index;

    // FINAL count of elements in *data
    size_t count;
};


struct ChunkMesh {
    struct Chunk *chunk;

    struct {
        // if true, this mesh will be depth sorted the next time it is rendered
        bool depth_sort: 1;

        // if true, this mesh will have its buffers kept in memory when rendered
        bool persist: 1;
    } flags;

    // data buffers
    struct ChunkMeshBuffer buffers[3];

    // total number of vertices in this mesh
    size_t vertex_count;

    // buffer objects
    struct VAO vao;
    struct VBO vbo, ibo;
};

enum ChunkMeshPass {
    FULL, TRANSPARENCY
};

struct ChunkMesh *chunkmesh_create(struct Chunk *chunk, bool depth_sort);
void chunkmesh_destroy(struct ChunkMesh *self);
void chunkmesh_render(struct ChunkMesh *self);
void chunkmesh_set_persist(struct ChunkMesh *self, bool persist);
void chunk_mesh(struct Chunk *self, enum ChunkMeshPass pass);
void chunk_depth_sort(struct Chunk *self);

#endif