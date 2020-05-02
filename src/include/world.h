#ifndef WORLD_H
#define WORLD_H

#include "util.h"
#include "gfx.h"
#include "chunk.h"
#include "block.h"
#include "player.h"

struct WorldUnloadedData {
    ivec3s pos;
    u32 data;
};

struct World {
    struct EntityPlayer player;

    // Size of one dimension of World::chunks
    size_t chunks_size;

    // Array of chunks in this world, size is always chunks_size ^ 3
    // Chunk pointers canbe NULL if they are not yet loaded
    struct Chunk **chunks;

    // Bottom-left offset of chunks array and offset of the center chunk
    ivec3s chunks_origin, center_offset;

    // Data which has been set in the world (i.e. by the world generator)
    // but is not yet in a loaded chunk
    struct {
        struct WorldUnloadedData *list;
        size_t size, capacity;
    } unloaded_data;

    struct {
        struct {
            u64 count, max;
        } mesh, load;
    } throttles;
};

// see worldgen.c
void worldgen_generate(struct Chunk *);

bool world_in_bounds(struct World *self, ivec3s pos);
bool world_contains(struct World *self, ivec3s pos);
struct Chunk *world_get_chunk(struct World *self, ivec3s offset);
ivec3s world_pos_to_block(vec3s pos);
ivec3s world_pos_to_offset(ivec3s pos);
ivec3s world_pos_to_chunk_pos(ivec3s pos);

void world_init(struct World *self);
void world_destroy(struct World *self);

void world_remove_unloaded_data(struct World *self, size_t i);

bool world_contains_chunk(struct World *self, ivec3s offset);
void world_set_center(struct World *self, ivec3s center_pos);
void world_render(struct World *self);
void world_update(struct World *self);
void world_tick(struct World *self);

#define WORLD_DECL_DATA(T, _name)\
    static inline T world_get_##_name(struct World *self, ivec3s pos) {\
        ivec3s offset = world_pos_to_offset(pos);\
        if (world_contains_chunk(self, offset)) {\
            return (T) chunk_get_##_name(\
                world_get_chunk(self, offset),\
                world_pos_to_chunk_pos(pos));\
        }\
        return 0;\
    }\
    static inline void world_set_##_name(struct World *self, ivec3s pos, T value) {\
        ivec3s offset = world_pos_to_offset(pos);\
        if (world_contains_chunk(self, offset)) {\
            chunk_set_##_name(\
                world_get_chunk(self, offset),\
                world_pos_to_chunk_pos(pos),\
                value);\
        }\
    }

WORLD_DECL_DATA(enum BlockId, block)
WORLD_DECL_DATA(u16, light)
WORLD_DECL_DATA(u8, sunlight)
WORLD_DECL_DATA(u32, all_light)
WORLD_DECL_DATA(u32, metadata)
WORLD_DECL_DATA(u64, data)

#endif