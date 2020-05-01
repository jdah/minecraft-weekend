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
        // if true, this chunk contains no blocks
        bool empty: 1;
    } flags;
    
    struct ChunkMesh *mesh;
};

enum ChunkMeshPart {
    BASE, TRANSPARENT
};

bool chunk_in_bounds(ivec3s pos);

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset);
void chunk_destroy(struct Chunk *self);
void chunk_set_data(struct Chunk *self, ivec3s pos, u32 data);
u32 chunk_get_data(struct Chunk *self, ivec3s pos);
void chunk_prepare(struct Chunk *self);
void chunk_render(struct Chunk *self, enum ChunkMeshPart part);
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

// Chunk mesh instance, should only be accessed on the main thread
struct ChunkMesh {
    struct Chunk *chunk;

    // data, indices, faces buffers
    struct ChunkMeshBuffer buffers[BUFFER_TYPE_LAST + 1];

    // total number of vertices in this mesh
    size_t vertex_count;

    struct {
        struct {
            size_t offset, count;
        } base, transparent;
    } indices;

    struct {
        // if true, this mesh needs to be finalized (uploaded)
        bool finalize: 1;

        // if true, this mesh will be rebuilt next time it is rendered
        bool dirty: 1;

        // if true, this mesh will be depth sorted next time it is rendered
        bool depth_sort: 1;

        // if true, this mesh will be destroyed when its data is next accessible
        bool destroy: 1;

        // if true, index and face buffers are kept in memory after building
        bool persist: 1;
    } flags;

    // buffer objects
    struct VAO vao;
    struct VBO vbo, ibo;
};

struct ChunkMesh *chunkmesh_create(struct Chunk *chunk);
void chunkmesh_destroy(struct ChunkMesh *self);
void chunkmesh_prepare_render(struct ChunkMesh *self);
void chunkmesh_render(struct ChunkMesh *self, enum ChunkMeshPart part);
void chunkmesh_set_persist(struct ChunkMesh *self, bool persist);

#endif