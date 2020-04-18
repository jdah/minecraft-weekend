#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

void air_init() {
    struct Block air = BLOCK_DEFAULT;
    air.is_transparent = is_transparent;
    BLOCKS[AIR] = air;
}