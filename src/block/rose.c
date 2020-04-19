#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}


static bool is_sprite() {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 3 }};
}

void rose_init() {
    struct Block rose = BLOCK_DEFAULT;
    rose.id = ROSE;
    rose.is_transparent = is_transparent;
    rose.is_sprite = is_sprite;
    rose.get_texture_location = get_texture_location;
    BLOCKS[ROSE] = rose;
}