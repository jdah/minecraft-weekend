#ifndef BLOCK_H
#define BLOCK_H

#include "util.h"
#include "blockatlas.h"

// Forward declaration
struct World;

enum BlockId {
    AIR = 0,
    GRASS = 1,
    DIRT = 2,
    STONE = 3,
    SAND = 4,
    WATER = 5,
    GLASS = 6,
    LOG = 7,
    LEAVES = 8,
    ROSE = 9,
    BUTTERCUP = 10,
    COAL = 11,
    COPPER = 12,
    LAVA = 13,
    CLAY = 14,
    GRAVEL = 15,
    PLANKS = 16
};

// Always the highest valid block id
#define BLOCK_ID_LAST PLANKS

struct Block {
    enum BlockId id;
    bool (*is_transparent)(struct World *world, ivec3s pos);
    ivec2s (*get_texture_location)(struct World *world, ivec3s pos, enum Direction d);
    bool (*is_animated)();
    void (*get_animation_frames)(ivec2s out[BLOCK_ATLAS_FRAMES]);
    bool (*is_sprite)();
    bool (*is_liquid)();
};

#define MAX_BLOCK_ID INT16_MAX

struct Block BLOCK_DEFAULT;
struct Block BLOCKS[MAX_BLOCK_ID];

void block_init();

#endif