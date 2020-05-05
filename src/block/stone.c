#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 3, 0 }};
}

void stone_init() {
    struct Block stone = BLOCK_DEFAULT;
    stone.id = STONE;
    stone.get_texture_location = get_texture_location;
    BLOCKS[STONE] = stone;
}