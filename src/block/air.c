#include "block.h"

void air_init() {
    struct Block air = BLOCK_DEFAULT;
    air.id = AIR;
    air.transparent = true;
    BLOCKS[AIR] = air;
}