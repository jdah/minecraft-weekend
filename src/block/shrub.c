#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 3, 3 }};
}

void shrub_init() {
    struct Block shrub = BLOCK_DEFAULT;
    shrub.id = SHRUB;
    shrub.transparent = true;
    shrub.solid = false;
    shrub.mesh_type = BLOCKMESH_SPRITE;
    shrub.get_texture_location = get_texture_location;
    BLOCKS[SHRUB] = shrub;
}