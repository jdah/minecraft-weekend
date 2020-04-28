#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 15 }};
}

static bool is_animated() {
    return true;
}

static bool is_liquid() {
    return true;
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        out[i] = (ivec2s) {{ i, 15 }}; 
    }
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