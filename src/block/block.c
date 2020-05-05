#include "block.h"
#include "../gfx/blockatlas.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    assert(false);
    return GLMS_IVEC2_ZERO;
}

static void get_animation_frames(ivec2s out[BLOCK_ATLAS_FRAMES]) {
    return;
}

static Torchlight get_torchlight(struct World *world, ivec3s pos) {
    return 0x0000;
}

// Contains all default behavior for a block
struct Block BLOCK_DEFAULT = {
    .id = -1,
    .transparent = false,
    .liquid = false,
    .can_emit_light = false,
    .animated = false,
    .mesh_type = BLOCKMESH_DEFAULT,
    .get_texture_location = get_texture_location,
    .get_animation_frames = get_animation_frames,
    .get_torchlight = get_torchlight
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
extern void torch_init();

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
    torch_init();
}