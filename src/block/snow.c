#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 3, 2 }};
}

void snow_init() {
    struct Block snow = BLOCK_DEFAULT;
    snow.id = SNOW;
    snow.get_texture_location = get_texture_location;
    BLOCKS[SNOW] = snow;
}