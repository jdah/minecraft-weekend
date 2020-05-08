#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 2, 2 }};
}

void cobblestone_init() {
    struct Block cobblestone = BLOCK_DEFAULT;
    cobblestone.id = COBBLESTONE;
    cobblestone.get_texture_location = get_texture_location;
    BLOCKS[COBBLESTONE] = cobblestone;
}