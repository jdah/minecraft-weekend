#include "block.h"
#include "../world/light.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    if (d == UP) {
        return (ivec2s) {{ 1, 2 }};
    }

    return (ivec2s) {{ 0, 2 }};
}

static Torchlight get_torchlight(struct World *world, ivec3s pos) {
    return TORCHLIGHT_OF(0xF, 0xB, 0x5, 0xF);
}

static void get_mesh_information(
    struct World *world, ivec3s pos, enum Direction d,
    vec3s *offset_out, vec3s *size_out,
    ivec2s *uv_offset_out, ivec2s *uv_size_out) {
    *offset_out = (vec3s) {{ 0.5f - (0.125f / 2.0f), 0.0f, 0.5f - (0.125f / 2.0f) }};
    *size_out = (vec3s) {{ 0.125f, (10.0f / 16.0f), 0.125f }};

    if (d == UP) {
        *uv_offset_out = (ivec2s) {{ 7, 7 }};
        *uv_size_out = (ivec2s) {{ 2, 2 }};
    } else {
        *uv_offset_out = (ivec2s) {{ 7, 0 }};
        *uv_size_out = (ivec2s) {{ 2, 10 }};
    }
}

void torch_init() {
    struct Block torch = BLOCK_DEFAULT;
    torch.id = TORCH;
    torch.transparent = true;
    torch.solid = false;
    torch.can_emit_light = true;
    torch.mesh_type = BLOCKMESH_CUSTOM;
    torch.get_texture_location = get_texture_location;
    torch.get_torchlight = get_torchlight;
    torch.get_mesh_information = get_mesh_information;
    BLOCKS[TORCH] = torch;
}