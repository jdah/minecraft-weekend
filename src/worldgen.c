#include "include/world.h"
#include "include/chunk.h"

#include <noise1234.h>

#define SRAND(seed) srand(seed)
#define RAND(min, max) ((rand() % (max - min + 1)) + min)
#define RANDCHANCE(chance) ((RAND(0, 100000) / 100000.0) <= chance)

#define RADIAL2I(c, r, v)\
    (glms_vec2_norm(glms_vec2_sub(IVEC2S2V((c)), IVEC2S2V((v)))) / glms_vec2_norm(IVEC2S2V((r))))

#define RADIAL3I(c, r, v)\
    (glms_vec3_norm(glms_vec3_sub(IVEC3S2V((c)), IVEC3S2V((v)))) / glms_vec3_norm(IVEC3S2V((r))))

#define WATER_LEVEL 64

enum Biome {
    OCEAN,
    PLAINS,
    BEACH,
    MOUNTAIN
};

typedef void (*FSet)(struct Chunk*, s32, s32, s32, u32);
typedef u32 (*FGet)(struct Chunk*, s32, s32, s32);

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

static u32 _get(struct Chunk *chunk, s32 x, s32 y, s32 z) {
    ivec3s p = (ivec3s) {{ x, y, z }};
    if (chunk_in_bounds(p)) {
        return chunk_get_data(chunk, p);
    } else {
        return world_get_data(chunk->world, glms_ivec3_add(chunk->position, p));
        return 0;
    }
}

static void _set(struct Chunk *chunk, s32 x, s32 y, s32 z, u32 d) {
    ivec3s p = (ivec3s) {{ x, y, z }};
    if (chunk_in_bounds(p)) {
        chunk_set_data(chunk, p, d);
    } else {
        world_set_data(chunk->world, glms_ivec3_add(chunk->position, p), d);
    }
}

void tree(struct Chunk *chunk, FGet get, FSet set, s32 x, s32 y, s32 z) {
    enum BlockId under = get(chunk, x, y - 1, z);
    if (under != GRASS && under != DIRT) {
        return;
    }

    s32 h = RAND(3, 5);

    for (s32 yy = y; yy <= (y + h); yy++) {
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

void flowers(struct Chunk *chunk, FGet get, FSet set, s32 x, s32 y, s32 z) {
    enum BlockId flower = RANDCHANCE(0.6) ? ROSE : BUTTERCUP;

    s32 s = RAND(2, 6);
    s32 l = RAND(s - 1, s + 1);
    s32 h = RAND(s - 1, s + 1);

    for (s32 xx = (x - l); xx <= (x + l); xx++) {
        for (s32 zz = (z - h); zz <= (z + h); zz++) {
            enum BlockId under = get(chunk, xx, y, zz);
            if ((under == GRASS) &&
                RANDCHANCE(0.5)) {
                set(chunk, xx, y + 1, zz, flower);
            }
        }   
    }
}

void orevein(struct Chunk *chunk, FGet get, FSet set, s32 x, s32 y, s32 z, enum BlockId block) {
    s32 h = RAND(1, y - 4);
    
    if (h < 0 || h > y - 4) {
        return;
    }

    s32 s;
    switch (block) {
        case COAL:
            s = RAND(2, 4);
            break;
        case COPPER:
        default:
            s = RAND(1, 3);
            break;
    }

    s32 l = RAND(s - 1, s + 1);
    s32 w = RAND(s - 1, s + 1);
    s32 i = RAND(s - 1, s + 1);

    for (s32 xx = (x - l); xx <= (x + l); xx++) {
        for (s32 zz = (z - w); zz <= (z + w); zz++) {
            for (s32 yy = (h - i); yy <= (h + i); yy++) {
                f32 d = 1.0f - RADIAL3I(
                    ((ivec3s) {{ x, h, z }}),
                    ((ivec3s) {{ l + 1, w + 1, i + 1 }}),
                    ((ivec3s) {{ xx, yy, zz }})
                );

                if (get(chunk, xx, yy, zz) == STONE && RANDCHANCE(0.2 + d * 0.7)) {
                    set(chunk, xx, yy, zz, block);
                }
            }
        }
    }
}

void lavapool(struct Chunk *chunk, FGet get, FSet set, s32 x, s32 y, s32 z) {
    s32 h = y - 1;

    s32 s = RAND(1, 5);
    s32 l = RAND(s - 1, s + 1);
    s32 w = RAND(s - 1, s + 1);

    for (s32 xx = (x - l); xx <= (x + l); xx++) {
        for (s32 zz = (z - w); zz <= (z + w); zz++) {
            f32 d = 1.0f - RADIAL2I(
                ((ivec2s) {{ x, z }}),
                ((ivec2s) {{ l + 1, w + 1 }}),
                ((ivec2s) {{ xx, zz }})
            );

            // all border blocks must be solid (or lava) to place lava
            bool allow = true;

            for (s32 i = -1; i <= 1; i++) {
                for (s32 j = -1; j <= 1; j++) {
                    enum BlockId block = get(chunk, xx + i, h, zz + j);
                    if (block != LAVA &&
                            BLOCKS[block].is_transparent(chunk->world, (ivec3s) {{ xx + i, h, zz + i }})) {
                        allow = false;
                        break;
                    }
                }
            }

            if (!allow) {
                continue;
            }

            if (RANDCHANCE(0.2 + d * 0.95)) {
                set(chunk, xx, h, zz, LAVA);
            }
        }
    }
}

void worldgen_generate(struct Chunk *chunk) {
    // TODO: configure in world.c
    const u64 seed = 2;
    SRAND(seed + ivec3shash(chunk->offset));

    // biome noise
    struct Noise n = octave(6, 0);

    // ore noise
    struct Noise m = octave(6, 1);

    // Different offsets of octave noise functions
    struct Noise os[] = {
        octave(8, 1),
        octave(8, 2),
        octave(8, 3),
        octave(8, 4),
        octave(8, 5),
        octave(8, 6),
    };

    // Two separate combined noise functions, each combining two different
    // octave noise functions
    struct Noise cs[] = {
        combined(&os[0], &os[1]),
        combined(&os[2], &os[3]),
        combined(&os[4], &os[5]),
    };

    for (s32 x = 0; x < CHUNK_SIZE.x; x++) {
        for (s32 z = 0; z < CHUNK_SIZE.z; z++) {
            s32 wx = chunk->position.x + x, wz = chunk->position.z + z;

            // Sample each combined noise function for high/low results
            const f32 base_scale = 1.3f;
            int hr;
            int hl = (cs[0].compute(&cs[0].params, seed, wx * base_scale, wz * base_scale) / 6.0f) - 4.0f;
            int hh = (cs[1].compute(&cs[1].params, seed, wx * base_scale, wz * base_scale) / 5.0f) + 6.0f;

            // Sample the biome noise and extra noise
            f32 t = n.compute(&n.params, seed, wx, wz);
            f32 r = m.compute(&m.params, seed, wx / 4.0f, wz / 4.0f) / 32.0f;

            if (t > 0) {
                hr = hl;
            } else {
                hr = max(hh, hl);
            }

            // offset by water level and determine biome
            s32 h = hr + WATER_LEVEL;

            // beach is anything close-ish to water in biome AND height
            enum Biome biome;
            if (h < WATER_LEVEL) {
                biome = OCEAN;
            } else if (t < 0.08f && h < WATER_LEVEL + 2) {
                biome = BEACH;
            } else if (false) {
                biome = MOUNTAIN;
            } else {
                biome = PLAINS;
            }

            if (biome == MOUNTAIN) {
                h += (r + (-t / 12.0f)) * 2 + 2;
            }

            // dirt or sand depth
            s32 d = r * 1.4f + 5.0f;

            enum BlockId top_block;
            switch (biome) {
                case OCEAN:
                    if (r > 0.8f) {
                        top_block = GRAVEL;
                    } else if (r > 0.3f) {
                        top_block = SAND;
                    } else if (r > 0.15f && t < 0.08f) {
                        top_block = CLAY;
                    } else {
                        top_block = DIRT;
                    }
                    break;
                case BEACH:
                    top_block = SAND;
                    break;
                case PLAINS:
                    top_block = (t > 4.0f && r > 0.78f) ? GRAVEL : GRASS;
                    break;
                case MOUNTAIN:
                    if (r > 0.8f) {
                        top_block = GRAVEL;
                    } else if (r > 0.7f) {
                        top_block = DIRT;
                    } else {
                        top_block = STONE;
                    }
                    break;
            }


            for (s32 y = 0; y < h; y++) {
                enum BlockId block;
                if (y == (h - 1)) {
                    block = top_block;
                } else if (y > (h - d)) {
                    if (top_block == GRASS) {
                        block = DIRT;
                    } else {
                        block = top_block;
                    }
                } else {
                    block = STONE;
                }

                chunk_set_data(chunk, (ivec3s) {{ x, y, z }}, block);
            }

            // fill up to water level with water
            for (s32 y = h; y < WATER_LEVEL; y++) {
                chunk_set_data(chunk, (ivec3s) {{ x, y, z }}, WATER);
            }

            if (RANDCHANCE(0.02)) {
                orevein(chunk, _get, _set, x, h, z, COAL);
            }

            if (RANDCHANCE(0.02)) {
                orevein(chunk, _get, _set, x, h, z, COPPER);
            }

            if (biome != OCEAN && h <= (WATER_LEVEL + 3) && t < 0.1f && RANDCHANCE(0.001)) {
                lavapool(chunk, _get, _set, x, h, z);
            }

            if (biome == PLAINS && RANDCHANCE(0.005)) {
                tree(chunk, _get, _set, x, h, z);
            }

            if (biome == PLAINS && RANDCHANCE(0.0085)) {
                flowers(chunk, _get, _set, x, h, z);
            }
        }
    }

    // set data in this chunk which was previously unloaded
    for (size_t i = 0; i < chunk->world->unloaded_data.size; i++) {
        struct WorldUnloadedData data = chunk->world->unloaded_data.list[i];
        if (!ivec3scmp(chunk->offset, world_pos_to_offset(data.pos))) {
            chunk_set_data(chunk, world_pos_to_chunk_pos(data.pos), data.data);
            world_remove_unloaded_data(chunk->world, i);
        }
    }
}