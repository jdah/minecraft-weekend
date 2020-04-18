#ifndef BLOCK_H
#define BLOCK_H

#include "util.h"

// Forward declaration
struct World;

enum BlockId {
    AIR = 0,
    GRASS = 1,
    DIRT = 2,
    STONE = 3,
    SAND = 4
};

struct Block {
    enum BlockId id;
    bool (*is_transparent)(struct World *world, ivec3s pos);
    ivec2s (*get_texture_location)(struct World *world, ivec3s pos, enum Direction d);
};

#define MAX_BLOCK_ID INT16_MAX

struct Block BLOCK_DEFAULT;
struct Block BLOCKS[MAX_BLOCK_ID];

void block_init();

#endif