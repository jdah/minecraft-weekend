#include "../include/block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 14 }};
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        out[i] = (ivec2s) {{ 14, i }};
    }
}

void lava_init() {
    struct Block lava = BLOCK_DEFAULT;
    lava.id = LAVA;
    lava.transparent = true;
    lava.animated = true;
    lava.liquid = true;
    lava.mesh_type = BLOCKMESH_LIQUID;
    lava.get_texture_location = get_texture_location;
    lava.get_animation_frames = get_animation_frames;
    BLOCKS[LAVA] = lava;
}