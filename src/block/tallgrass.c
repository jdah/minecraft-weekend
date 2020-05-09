#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 2, 3 }};
}

void tallgrass_init() {
    struct Block tallgrass = BLOCK_DEFAULT;
    tallgrass.id = TALLGRASS;
    tallgrass.transparent = true;
    tallgrass.solid = false;
    tallgrass.mesh_type = BLOCKMESH_SPRITE;
    tallgrass.get_texture_location = get_texture_location;
    BLOCKS[TALLGRASS] = tallgrass;
}