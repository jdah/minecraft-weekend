#ifndef CHUNK_H
#define CHUNK_H

#include "../block/block.h"
#include "../util/util.h"
#include "chunkmesh.h"
#include "light.h"

// forward declarations
struct World;

#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32

// max(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
#define CHUNK_SIZE_MAX 32

#define CHUNK_SIZE_XZ ((ivec2s) {{ CHUNK_SIZE_X, CHUNK_SIZE_Z }})

#define CHUNK_SIZE ((ivec3s) {{ CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z }})
#define CHUNK_SIZE_F ((vec3s) {{ CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z }})

#define CHUNK_VOLUME (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

#define chunk_foreach(_pname)\
    ivec3s _pname = GLMS_IVEC3_ZERO_INIT;\
    for (s32 x = 0; x < CHUNK_SIZE.x; x++)\
        for (s32 z = 0; z < CHUNK_SIZE.z; z++)\
            for (s32 y = 0;\
                y < CHUNK_SIZE.y &&\
                ((_pname.x = x) != INT32_MAX) &&\
                ((_pname.y = y) != INT32_MAX) &&\
                ((_pname.z = z) != INT32_MAX);\
                y++)

#define chunk_pos_to_index(p) (p.x * CHUNK_SIZE.x * CHUNK_SIZE.z + p.z * CHUNK_SIZE.z + p.y)

// returns true if pos is within chunk boundaries
static inline bool chunk_in_bounds(ivec3s pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 &&
        pos.x < CHUNK_SIZE.x && pos.y < CHUNK_SIZE.y && pos.z < CHUNK_SIZE.z;
}

// returns true if pos is on chunk boundaries (borders another chunk)
static inline bool chunk_on_bounds(ivec3s pos) {
    return pos.x == 0 || pos.y == 0 || pos.z == 0 ||
        pos.x == (CHUNK_SIZE.x - 1) || pos.y == (CHUNK_SIZE.y - 1) || pos.z == (CHUNK_SIZE.z - 1);
}

struct Chunk {
    struct World *world;

    // offset (in chunks) and position (in blocks)
    ivec3s offset, position;

    // chunk data stored in a 3D array
    // each u64 has the following bit fields:
    // - 28 bits metadata/extra
    // - 4 bits sunlight intensity
    // - 4 bits R light
    // - 4 bits G light
    // - 4 bits B light
    // - 4 bits light intensity
    // - 16 bits block id
    u64 *data;

    // number of blocks
    size_t count;

    struct {
        // if true, this chunk contains no blocks
        bool empty: 1;

        // if true, this chunk is generating
        bool generating: 1;
    } flags;
    
    struct ChunkMesh *mesh;
};

bool chunk_in_bounds(ivec3s pos);

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset);
void chunk_destroy(struct Chunk *self);
void chunk_prepare(struct Chunk *self);
void chunk_render(struct Chunk *self, enum ChunkMeshPart part);
void chunk_update(struct Chunk *self);
void chunk_tick(struct Chunk *self);

void chunk_after_generate(struct Chunk *self);
void chunk_on_modify(
    struct Chunk *self, ivec3s pos,
    u64 prev, u64 data);

#define BLOCK_MASK 0x000000000000FFFF
#define BLOCK_OFFSET 0

#define TORCHLIGHT_MASK 0x000000000FFFF0000
#define TORCHLIGHT_OFFSET 16

#define SUNLIGHT_MASK 0x0000000F00000000
#define SUNLIGHT_OFFSET 32

#define LIGHT_MASK 0x0000000FFFFF0000
#define LIGHT_OFFSET 16

#define METADATA_MASK 0xFFFFFF7000000000
#define METADATA_OFFSET 36

#define DATA_MASK 0xFFFFFFFFFFFFFFFF
#define DATA_OFFSET 0

#define CHUNK_DECL_DATA(T, _name, _mask, _offset)\
    static inline T chunk_get_##_name(\
        struct Chunk *self,\
        ivec3s pos) {\
            return (T) ((self->data[chunk_pos_to_index(pos)] & _mask) >> _offset);\
        }\
    static inline void chunk_set_##_name(\
        struct Chunk *self,\
        ivec3s pos,\
        T value) {\
            size_t index = chunk_pos_to_index(pos);\
            u64 data = self->data[index];\
            u64 new_data = (data & ~(_mask)) |\
                ((((u64) value) << _offset) & _mask);\
            self->data[index] = new_data;\
            chunk_on_modify(self, pos, data, new_data);\
        }\
    static inline T chunk_data_to_##_name(\
        u64 value) {\
            return (T) ((value & _mask) >> _offset);\
        }

CHUNK_DECL_DATA(enum BlockId, block, BLOCK_MASK, BLOCK_OFFSET)
CHUNK_DECL_DATA(Torchlight, torchlight, TORCHLIGHT_MASK, TORCHLIGHT_OFFSET)
CHUNK_DECL_DATA(Sunlight, sunlight, SUNLIGHT_MASK, SUNLIGHT_OFFSET)
CHUNK_DECL_DATA(Light, light, LIGHT_MASK, LIGHT_OFFSET)
CHUNK_DECL_DATA(u32, metadata, METADATA_MASK, METADATA_OFFSET)
CHUNK_DECL_DATA(u64, data, DATA_MASK, DATA_OFFSET)

#endif