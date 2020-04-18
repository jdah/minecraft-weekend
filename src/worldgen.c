#include "include/world.h"
#include "include/chunk.h"

void worldgen_generate(struct Chunk *chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int h = 64;
            for (int y = 0; y < h; y++) {
                enum BlockId type = AIR;
                if (y == h - 1) {
                    type = GRASS;
                } else if (y > (h - 3)) {
                    type = DIRT;
                } else {
                    type = STONE;
                }

                chunk_set_data(chunk, (ivec3s) {{ x, y, z }}, type);
            }
        }
    }
}