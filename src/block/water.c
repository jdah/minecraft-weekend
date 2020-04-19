#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 2 }};
}

static bool is_animated() {
    return true;
}

static bool is_liquid() {
    return true;
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    out[0] = (ivec2s) {{ 0, 2 }};
    out[1] = (ivec2s) {{ 1, 2 }};
    out[2] = (ivec2s) {{ 2, 2 }};
    out[3] = (ivec2s) {{ 3, 2 }};
    out[4] = (ivec2s) {{ 4, 2 }};
}


void water_init() {
    struct Block water = BLOCK_DEFAULT;
    water.id = WATER;
    water.is_transparent = is_transparent;
    water.get_texture_location = get_texture_location;
    water.is_animated = is_animated;
    water.get_animation_frames = get_animation_frames;
    water.is_liquid = is_liquid;
    BLOCKS[WATER] = water;
}