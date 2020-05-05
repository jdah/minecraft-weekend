#include "../include/block.h"
#include "../include/light.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    return (ivec2s) {{ 0, 2 }};
}

static Torchlight get_torchlight(struct World *world, ivec3s pos) {
    return TORCHLIGHT_OF(0xF, 0xF, 0xF, 0xF);
}

void torch_init() {
    struct Block torch = BLOCK_DEFAULT;
    torch.id = TORCH;
    torch.transparent = true;
    torch.can_emit_light = true;
    torch.mesh_type = BLOCKMESH_TORCH;
    torch.get_texture_location = get_texture_location;
    torch.get_torchlight = get_torchlight;
    BLOCKS[TORCH] = torch;
}