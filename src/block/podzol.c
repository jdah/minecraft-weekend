#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    switch (d) {
        case UP:
            return (ivec2s) {{ 4, 2 }};
        case DOWN:
            return (ivec2s) {{ 2, 0 }};
        default:
            return (ivec2s) {{ 5, 2 }};
    }
}

void podzol_init() {
    struct Block podzol = BLOCK_DEFAULT;
    podzol.id = PODZOL;
    podzol.get_texture_location = get_texture_location;
    BLOCKS[PODZOL] = podzol;
}