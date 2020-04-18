#include "include/block.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return false;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    assert(false);
    return GLMS_IVEC2_ZERO;
}

// Contains all default behavior for a block
struct Block BLOCK_DEFAULT = {
    .id = -1,
    .is_transparent = is_transparent,
    .get_texture_location = get_texture_location
};

extern void air_init();
extern void grass_init();
extern void dirt_init();
extern void stone_init();
extern void sand_init(); 
extern void water_init();
extern void glass_init();

void block_init() {
    air_init();
    grass_init();
    dirt_init();
    stone_init();
    sand_init();
    water_init();
    glass_init();
}