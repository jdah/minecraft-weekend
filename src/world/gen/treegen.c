#include "worldgen.h"
#include "../world.h"

void worldgen_tree(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z) {
    enum BlockId under = get(chunk, x, y, z);
    if (under != GRASS && under != DIRT) {
        return;
    }

    s32 h = RAND(3, 5);

    for (s32 yy = y + 1; yy <= (y + h); yy++) {
        set(chunk, x, yy, z, LOG);
    }

    s32 lh = RAND(2, 3);

    for (s32 xx = (x - 2); xx <= (x + 2); xx++) {
        for (s32 zz = (z - 2); zz <= (z + 2); zz++) {
            for (s32 yy = (y + h); yy <= (y + h + 1); yy++) {
                s32 c = 0;
                c += xx == (x - 2) || xx == (x + 2);
                c += zz == (z - 2) || zz == (z + 2);
                bool corner = c == 2;

                if ((!(xx == x && zz == z) || yy > (y + h)) &&
                    !(corner && yy == (y + h + 1) && RANDCHANCE(0.4))) {
                    set(chunk, xx, yy, zz, LEAVES);
                }
            }
        }
    }

    for (s32 xx = (x - 1); xx <= (x + 1); xx++) {
        for (s32 zz = (z - 1); zz <= (z + 1); zz++) {
            for (s32 yy = (y + h + 2); yy <= (y + h + lh); yy++) {
                s32 c = 0;
                c += xx == (x - 1) || xx == (x + 1);
                c += zz == (z - 1) || zz == (z + 1);
                bool corner = c == 2;

                if (!(corner && yy == (y + h + lh) && RANDCHANCE(0.8))) {
                    set(chunk, xx, yy, zz, LEAVES);
                }
            }
        }
    }
}

void worldgen_pine(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z) {
    enum BlockId under = get(chunk, x, y, z);
    if (under != GRASS && under != DIRT && under != SNOW && under != PODZOL) {
        return;
    }

    s64 h = RAND(5, 7);

    for (s32 yy = y + 1; yy <= (y + h); yy++) {
        set(chunk, x, yy, z, PINE_LOG);
    }

    s64 r = RAND(2, 3);

    for(s64 yy = (y + 3); yy <= (y + h); yy++) {
        if (yy != (y + h) && yy % 2 == 0) {
            continue;
        }

        if (yy == (y + h)) {
            r = 1;
        } else if (r > 1 && RANDCHANCE(0.7)) {
            r--;
        }

        for (s32 xx = (x - r); xx <= (x + r); xx++) {
            for (s32 zz = (z - r); zz <= (z + r); zz++) {
                s32 c = 0;
                c += xx == (x - r) || xx == (x + r);
                c += zz == (z - r) || zz == (z + r);
                bool corner = c == 2;

                if (!corner && !(xx == r && zz == r)) {
                    set(chunk, xx, yy, zz, PINE_LEAVES);
                }
            }
        }
    }

    set(chunk, x, y + h + 1, z, PINE_LEAVES);
}