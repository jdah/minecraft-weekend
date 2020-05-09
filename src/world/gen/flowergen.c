#include "worldgen.h"
#include "../world.h"

void worldgen_flowers(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z) {
    enum BlockId flower = RANDCHANCE(0.6) ? ROSE : BUTTERCUP;

    s64 s = RAND(2, 6);
    s64 l = RAND(s - 1, s + 1);
    s64 h = RAND(s - 1, s + 1);

    for (s64 xx = (x - l); xx <= (x + l); xx++) {
        for (s64 zz = (z - h); zz <= (z + h); zz++) {
            enum BlockId block = get(chunk, xx, y + 1, zz),
                under = get(chunk, xx, y, zz);
            if (block == AIR &&
                under == GRASS &&
                RANDCHANCE(0.5)) {
                set(chunk, xx, y + 1, zz, flower);
            }
        }
    }
}