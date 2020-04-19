#include "include/block.h"
#include "include/blockatlas.h"

static bool is_transparent(struct World *world, ivec3s pos) {
    return false;
}

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    assert(false);
    return GLMS_IVEC2_ZERO;
}

static bool is_animated() {
    return false;
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    return;
}

static bool is_sprite() {
    return false;
}

static bool is_liquid() {
    return false;
}

// Contains all default behavior for a block
struct Block BLOCK_DEFAULT = {
    .id = -1,
    .is_transparent = is_transparent,
    .get_texture_location = get_texture_location,
    .is_animated = is_animated,
    .get_animation_frames = get_animation_frames,
    .is_sprite = is_sprite,
    .is_liquid = is_liquid
};

extern void air_init();
extern void grass_init();
extern void dirt_init();
extern void stone_init();
extern void sand_init(); 
extern void water_init();
extern void glass_init();
extern void log_init();
extern void leaves_init();
extern void rose_init();
extern void buttercup_init();
extern void coal_init();
extern void copper_init();
extern void lava_init();
extern void clay_init();
extern void gravel_init();
extern void planks_init();

void block_init() {
    air_init();
    grass_init();
    dirt_init();
    stone_init();
    sand_init();
    water_init();
    glass_init();
    log_init();
    leaves_init();
    rose_init();
    buttercup_init();
    coal_init();
    copper_init();
    lava_init();
    clay_init();
    gravel_init();
    planks_init();
}