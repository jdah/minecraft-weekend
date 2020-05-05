#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 6, 1 }};
}

void planks_init() {
    struct Block planks = BLOCK_DEFAULT;
    planks.id = PLANKS;
    planks.get_texture_location = get_texture_location;
    BLOCKS[PLANKS] = planks;
}