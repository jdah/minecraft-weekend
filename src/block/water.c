#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 15 }};
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        out[i] = (ivec2s) {{ i, 15 }}; 
    }
}

void water_init() {
    struct Block water = BLOCK_DEFAULT;
    water.id = WATER;
    water.transparent = true;
    water.animated = true;
    water.liquid = true;
    water.solid = false;
    water.gravity_modifier = 0.72f;
    water.drag = 10.0f;
    water.mesh_type = BLOCKMESH_LIQUID;
    water.get_texture_location = get_texture_location;
    water.get_animation_frames = get_animation_frames;
    BLOCKS[WATER] = water;
}