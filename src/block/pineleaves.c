#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 6, 3 }};
}

void pine_leaves_init() {
    struct Block pine_leaves = BLOCK_DEFAULT;
    pine_leaves.id = PINE_LEAVES;
    pine_leaves.transparent = true;
    pine_leaves.get_texture_location = get_texture_location;
    BLOCKS[PINE_LEAVES] = pine_leaves;
}