#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 4, 0 }};
}

void coal_init() {
    struct Block coal = BLOCK_DEFAULT;
    coal.id = COAL;
    coal.get_texture_location = get_texture_location;
    BLOCKS[COAL] = coal;
}