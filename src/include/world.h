#ifndef WORLD_H
#define WORLD_H

#include "util.h"
#include "gfx.h"
#include "chunk.h"
#include "block.h"
#include "player.h"
#include "sky.h"

#define DAY_TICKS (14 * 60 * TICKRATE)
#define NIGHT_TICKS (10 * 60 * TICKRATE)
#define TOTAL_DAY_TICKS (DAY_TICKS + NIGHT_TICKS)

struct WorldUnloadedData {
    ivec3s pos;
    u32 data;
};

struct WorldgenData {
    s64 h;
    f32 t, r;
};

// LONG_MIN is used for the unknown value so everything is greater than it
// this means world_heightmap_update will always pick a new value over
// HEIGHTMAP_UNKNOWN
#define HEIGHTMAP_UNKNOWN LONG_MIN
struct Heightmap {
    // X-Z offset
    ivec2s offset;

    // heightmap data (highest block in this Y column)
    s64 *data;

    // heightmap worldgen data (see worldgen.c for usage)
    struct WorldgenData *worldgen_data;

    struct {
        bool generated: 1; 
    } flags;
};

// computes the index of position (x, z) in a heightmap
#define HEIGHTMAP_INDEX(p) ({ ivec2s q = (p); (q.x * CHUNK_SIZE.x) + q.y; })

// gets heightmap data from heightmap h at the specified (x, z) position
#define HEIGHTMAP_GET(h, p) ((h)->data[HEIGHTMAP_INDEX(p)])

// sets heightmap data for heightmap h at the specified (x, z) position
#define HEIGHTMAP_SET(h, p, y) ((h)->data[HEIGHTMAP_INDEX(p)] = y)

struct World {
    struct Sky sky;
    struct EntityPlayer player;

    // tick counter
    u64 ticks;

    u64 seed;

    // Size of one dimension of World::chunks
    size_t chunks_size;

    // Array of chunks in this world, size is always chunks_size ^ 3
    // Chunk pointers canbe NULL if they are not yet loaded
    struct Chunk **chunks;

    // Bottom-left offset of chunks array and offset of the center chunk
    ivec3s chunks_origin, center_offset;
    
    // Array of heightmaps for each X-Z chunk column, sizeis always chunks_size ^ 2
    struct Heightmap **heightmaps;

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

struct Heightmap *chunk_get_heightmap(struct Chunk *self);
s64 world_heightmap_get(struct World *self, ivec2s p);
bool world_heightmap_update(struct World *self, ivec3s p);
void world_heightmap_recalculate(struct World *self, ivec2s p);

void world_init(struct World *self);
void world_destroy(struct World *self);
void world_remove_unloaded_data(struct World *self, size_t i);
void world_set_center(struct World *self, ivec3s center_pos);
void world_render(struct World *self);
void world_update(struct World *self);
void world_tick(struct World *self);

// block position -> chunk offset
static inline ivec3s world_pos_to_offset(ivec3s pos) {
    return (ivec3s) {{
        (s32) floorf(pos.x / CHUNK_SIZE_F.x),
        (s32) floorf(pos.y / CHUNK_SIZE_F.y),
        (s32) floorf(pos.z / CHUNK_SIZE_F.z)
    }};
}

// float pos -> block pos
static inline ivec3s world_pos_to_block(vec3s pos) {
    return (ivec3s) {{
        (s32) floorf(pos.x),
        (s32) floorf(pos.y),
        (s32) floorf(pos.z)
    }};
}

// world position -> chunk position
static inline ivec3s world_pos_to_chunk_pos(ivec3s pos) {
    // ((pos % size) + size) % size
    return glms_ivec3_mod(glms_ivec3_add(glms_ivec3_mod(pos, CHUNK_SIZE), CHUNK_SIZE), CHUNK_SIZE);
}

static inline bool world_chunk_in_bounds(struct World *self, ivec3s offset) {
    ivec3s p = glms_ivec3_sub(offset, self->chunks_origin);
    return p.x >= 0 && p.y >= 0 && p.z >= 0 &&
        p.x < (s32) self->chunks_size && p.y < (s32) self->chunks_size && p.z < (s32) self->chunks_size;
}

// chunk offset -> world array index
static inline size_t world_chunk_index(struct World *self, ivec3s offset) {
    ivec3s p = glms_ivec3_sub(offset, self->chunks_origin);
    return (p.x * self->chunks_size * self->chunks_size) + (p.y * self->chunks_size) + p.z;
}

// world array index -> chunk offset
static inline ivec3s world_chunk_offset(struct World *self, size_t i) {
    return glms_ivec3_add(
        self->chunks_origin,
        (ivec3s) {{
            i / (self->chunks_size * self->chunks_size),
            (i / self->chunks_size) % self->chunks_size,
            i % self->chunks_size
        }}
    );
}

// returns the chunk at the specified offset, NULL if it is not loaded or is
// out of bounds
static inline struct Chunk *world_get_chunk(struct World *self, ivec3s offset) {
    if (!world_chunk_in_bounds(self, offset)) {
        return NULL;
    } else {
        return self->chunks[world_chunk_index(self, offset)];
    }
}

// returns true if the specified chunk offset is loaded
static inline bool world_contains_chunk(struct World *self, ivec3s offset) {
    return world_get_chunk(self, offset) != NULL;
}

// returns true if the specified position is in bounds (loaded or unloaded)
static inline bool world_in_bounds(struct World *self, ivec3s pos) {
    return world_chunk_in_bounds(self, world_pos_to_offset(pos));
}

// returns true if the world contains the specified position (it is loaded)
static inline bool world_contains(struct World *self, ivec3s pos) {
    return world_contains_chunk(self, world_pos_to_offset(pos));
}

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