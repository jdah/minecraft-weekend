#include "block.h"

static ivec2s get_texture_location(struct World *world, ivec3s pos, enum Direction d) {
    switch (d) {
        case UP:
        case DOWN:
            return (ivec2s) {{ 3, 1 }};
        default:
            return (ivec2s) {{ 2, 1 }}; 
    }
}

void log_init() {
    struct Block log = BLOCK_DEFAULT;
    log.id = LOG;
    log.get_texture_location = get_texture_location;
    BLOCKS[LOG] = log;
}