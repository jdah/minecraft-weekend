#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 2 }};
}

void water_init() {
    struct Block water = BLOCK_DEFAULT;
    water.id = WATER;
    water.is_transparent = is_transparent;
    water.get_texture_location = get_texture_location;
    BLOCKS[WATER] = water;
}