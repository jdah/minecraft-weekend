#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 14 }};
}

static bool is_animated() {
    return true;
}

static bool is_liquid() {
    return true;
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        out[i] = (ivec2s) {{ 14, i }};
    }
}

void lava_init() {
    struct Block lava = BLOCK_DEFAULT;
    lava.id = LAVA;
    lava.is_transparent = is_transparent;
    lava.get_texture_location = get_texture_location;
    lava.is_animated = is_animated;
    lava.get_animation_frames = get_animation_frames;
    lava.is_liquid = is_liquid;
    BLOCKS[LAVA] = lava;
}