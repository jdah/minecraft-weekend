#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 5, 0 }};
}

void copper_init() {
    struct Block copper = BLOCK_DEFAULT;
    copper.id = COPPER;
    copper.get_texture_location = get_texture_location;
    BLOCKS[COPPER] = copper;
}