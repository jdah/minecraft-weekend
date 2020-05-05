#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "../gfx/gfx.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"
#include "../util/util.h"

// forward declarations
struct Chunk;

enum ChunkMeshPart {
    BASE,
    TRANSPARENT
};

#define BUFFER_TYPE_LAST FACES
enum BufferType {
    DATA = 0,
    INDICES,
    FACES
};

struct Face {
    // index of face indices in 'indices' ChunkMeshBuffer
    size_t indices_base;

    // position of face (used for sorting)
    vec3s position;

    // precomputed distance (used for sorting)
    f32 distance;
};

struct ChunkMeshBuffer {
    enum BufferType type;

    // data store for this buffer, NULL if not currently allocated
    void *data;

    // capacity (in bytes) of data
    size_t capacity;

    // current index (in bytes) into data, used when building mesh
    size_t index;

    // final count (in bytes) in data
    size_t count;

    // current count (in elements) of data
    size_t elements;
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
        bool finalize : 1;

        // if true, this mesh will be rebuilt next time it is rendered
        bool dirty : 1;

        // if true, this mesh will be depth sorted next time it is rendered
        bool depth_sort : 1;

        // if true, this mesh will be destroyed when its data is next accessible
        bool destroy : 1;

        // if true, index and face buffers are kept in memory after building
        bool persist : 1;
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