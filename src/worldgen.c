#include "include/world.h"
#include "include/chunk.h"

#include <noise1234.h>

#define SRAND(seed) srand(seed)
#define RAND(min, max) ((rand() % (max - min + 1)) + min)
#define RANDCHANCE(chance) ((RAND(0, 100) / 100.0f) <= chance)

#define WATER_LEVEL 64

enum Biome {
    OCEAN,
    PLAINS,
    BEACH
};

typedef f32 (*FNoise)(void *p, f32 s, f32 x, f32 z);

struct Noise {
    u8 params[512]; // either Octave or Combined
    FNoise compute;
};

// Octave noise with n octaves and seed offset o
// Maximum amplitude is 2^0 + 2^1 + 2^2 ... 2^n = 2^(n+1) - 1
// i.e. for octave 8, values range between [-511, 511]
struct Octave {
    s32 n, o;
};

// Combined noise where compute(x, z) = n.compute(x + m.compute(x, z), z)
struct Combined {
    struct Noise *n, *m;
};

f32 octave_compute(struct Octave *p, f32 seed, f32 x, f32 z) {
    f32 u = 1.0f, v = 0.0f;
    for (int i = 0; i < p->n; i++) {
        v += noise3(x / u, z / u, seed + i + (p->o * 32)) * u;
        u *= 2.0f;
    }
    return v;
}

struct Noise octave(s32 n, s32 o) {
    struct Noise result = { .compute = (FNoise) octave_compute };
    struct Octave params = { n, o };
    memcpy(&result.params, &params, sizeof(struct Octave));
    return result;
}

f32 combined_compute(struct Combined *p, f32 seed, f32 x, f32 z) {
    return p->n->compute(&p->n->params, seed, x + p->m->compute(&p->m->params, seed, x, z), z);
}

struct Noise combined(struct Noise *n, struct Noise *m) {
    struct Noise result = { .compute = (FNoise) combined_compute };
    struct Combined params = { n, m };
    memcpy(&result.params, &params, sizeof(struct Combined));
    return result;
}

void worldgen_generate(struct Chunk *chunk) {
    // TODO: configure in world.c
    const u64 seed = 1;

    // Base noise
    struct Noise n = octave(6, 0);

    // Different offsets of octave noise functions
    struct Noise os[] = {
        octave(8, 1),
        octave(8, 2),
        octave(8, 3),
        octave(8, 4),
    };

    // Two separate combined noise functions, each combining two different
    // octave noise functions
    struct Noise cs[] = {
        combined(&os[0], &os[1]),
        combined(&os[2], &os[3]),
    };

    for (s32 x = 0; x < CHUNK_SIZE.x; x++) {
        for (s32 z = 0; z < CHUNK_SIZE.z; z++) {
            s32 wx = chunk->position.x + x, wz = chunk->position.z + z;

            // Sample each combined noise function for high/low results
            const f32 base_scale = 1.3f;
            int hr;
            int hl = (cs[0].compute(&cs[0].params, seed, wx * base_scale, wz * base_scale) / 6.0f) - 4.0f;
            int hh = (cs[1].compute(&cs[1].params, seed, wx * base_scale, wz * base_scale) / 5.0f) + 6.0f;

            // Sample the "biome" noise
            f32 t = n.compute(&n.params, seed, wx, wz);

            if (t > 0) {
                hr = hl;
            } else {
                hr = max(hh, hl);
            }

            // offset by water level and determine biome
            int h = hr + WATER_LEVEL;

            // beach is anything close-ish to water in biome AND height
            enum Biome biome = (h < WATER_LEVEL ?
                OCEAN :
                ((t < 0.08f && h < WATER_LEVEL + 2) ? BEACH : PLAINS));

            for (int y = 0; y < h; y++) {
                enum BlockId block;
                if (y == (h - 1)) {
                    // Determine top block according to biome
                    switch (biome) {
                        case OCEAN:
                            // put sand floors in some places in the ocean
                            block = (t > 0.03f ? DIRT : SAND);
                            break;
                        case BEACH:
                            block = SAND;
                            break;
                        case PLAINS:
                            block = GRASS;
                            break;
                    }
                } else if (y > (h - 4)) {
                    // TODO: varying dirt layer height
                    block = biome == BEACH ? SAND : DIRT;
                } else {
                    block = STONE;
                }

                chunk_set_data(chunk, (ivec3s) {{ x, y, z }}, block);
            }

            // TODO: water
            // TODO: trees
            // TODO: flowers
        }
    }
}