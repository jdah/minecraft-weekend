#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

static bool is_sprite() {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 1, 3 }};
}

void buttercup_init() {
    struct Block buttercup = BLOCK_DEFAULT;
    buttercup.id = BUTTERCUP;
    buttercup.is_transparent = is_transparent;
    buttercup.is_sprite = is_sprite;
    buttercup.get_texture_location = get_texture_location;
    BLOCKS[BUTTERCUP] = buttercup;
}