#include "../include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return true;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 1, 1 }};
}

void glass_init() {
    struct Block glass = BLOCK_DEFAULT;
    glass.id = GLASS;
    glass.is_transparent = is_transparent;
    glass.get_texture_location = get_texture_location;
    BLOCKS[GLASS] = glass;
}