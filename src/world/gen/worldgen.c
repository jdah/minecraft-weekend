
#include "worldgen.h"
#include "noise.h"
#include "../chunk.h"
#include "../world.h"

#define RADIAL2I(c, r, v) \
    (glms_vec2_norm(glms_vec2_sub(IVEC2S2V((c)), IVEC2S2V((v)))) / glms_vec2_norm(IVEC2S2V((r))))

#define RADIAL3I(c, r, v) \
    (glms_vec3_norm(glms_vec3_sub(IVEC3S2V((c)), IVEC3S2V((v)))) / glms_vec3_norm(IVEC3S2V((r))))

#define WATER_LEVEL 0

#define BIOME_LAST MOUNTAIN
enum Biome {
    OCEAN,
    PLAINS,
    SHRUBLAND,
    SWAMP,
    BEACH,
    FOREST,
    JUNGLE,
    TUNDRA,
    DESERT,
    HILLS,
    TAIGA,
    MOUNTAIN
};

#define MAX_DECORATIONS 8
struct Decoration {
    FWGDecorate f;
    f32 chance;
};

struct BiomeData {
    enum BlockId top_block, under_block;
    f32 h, m, t, s, e, r, y;
    struct Decoration decorations[MAX_DECORATIONS];
};

struct BiomeData BIOME_DATA[BIOME_LAST + 1] = {
    [OCEAN] = {
        .top_block = SAND,
        .under_block = SAND,
        .h = 1000.0f,
        .m = 1000.0f,
        .t = 1000.0f,
        .s = 1.0f,
        .e = 1.0f,
        .r = NAN,
        .y = NAN
    },
    [PLAINS] = {
        .top_block = GRASS,
        .under_block = DIRT,
        .h = 0.1f,
        .m = 0.3f,
        .t = 0.3f,
        .s = 0.6f,
        .e = 0.95f,
        .r = 0.03f,
        .y = 0.25f,
        .decorations = {
            { .f = worldgen_tree, .chance = 0.002f },
            { .f = worldgen_flowers, .chance = 0.005f }
        }
    },
    [SHRUBLAND] = {
        .top_block = GRASS,
        .under_block = DIRT,
        .h = 0.1f,
        .m = 0.2f,
        .t = 0.7f,
        .s = 1.0f,
        .e = 1.0f,
        .r = 1.0f,
        .y = 0.2f
    },
    [SWAMP] = {
        .top_block = DIRT,
        .under_block = DIRT,
        .h = 0.05f,
        .m = 0.8f,
        .t = 0.3f,
        .s = 1.0f,
        .e = 1.0f,
        .r = 1.0f,
        .y = 0.2f,
        .decorations = {
            { .f = worldgen_tree, .chance = 0.02f },
        }
    },
    [BEACH] = {
        .top_block = SAND,
        .under_block = SAND,
        .h = 0.0f,
        .m = 0.3f,
        .t = 0.3f,
        .s = 0.4f,
        .e = 1.0f,
        .r = 0.05f,
        .y = 0.2f
    },
    [FOREST] = {
        .top_block = GRASS,
        .under_block = DIRT,
        .h = 0.12f,
        .m = 0.38f,
        .t = 0.28f,
        .s = 0.6f,
        .e = 0.95f,
        .r = 0.05f,
        .y = 0.26f,
        .decorations = {
            { .f = worldgen_tree, .chance = 0.0035f },
            { .f = worldgen_flowers, .chance = 0.003f }
        }
    },
    [JUNGLE] = {
        .top_block = GRASS,
        .under_block = DIRT,
        .h = 0.5f,
        .m = 0.8f,
        .t = 0.6f,
        .s = 1.0f,
        .e = 1.0f,
        .r = 1.0f,
        .y = 0.24f
    },
    [TUNDRA] = {
        .top_block = SNOW,
        .under_block = SNOW,
        .h = 0.2f,
        .m = 0.7f,
        .t = -0.3f,
        .s = 1.0f,
        .e = 1.0f,
        .r = 1.0f,
        .y = 0.25f
    },
    [DESERT] = {
        .top_block = SAND,
        .under_block = SAND,
        .h = 0.1f,
        .m = 0.15f,
        .t = 0.7f,
        .s = 1.0f,
        .e = 1.0f,
        .r = 1.0f,
        .y = 0.23f
    },
    [HILLS] = {
        .top_block = GRASS,
        .under_block = DIRT,
        .h = 0.19f,
        .m = 0.4f,
        .t = 0.4f,
        .s = 0.6f,
        .e = 0.8f,
        .r = 0.06f,
        .y = 0.30f,
        .decorations = {
            { .f = worldgen_tree, .chance = 0.0025f },
            { .f = worldgen_flowers, .chance = 0.003f }
        }
    },
    [TAIGA] = {
        .top_block = PODZOL,
        .under_block = DIRT,
        .h = 0.4f,
        .m = 0.5f,
        .t = 0.4f,
        .s = 0.6f,
        .e = 0.98f,
        .r = 0.10f,
        .y = 0.26f,
        .decorations = {
            { .f = worldgen_pine, .chance = 0.003f },
        }
    },
    [MOUNTAIN] = {
        .top_block = STONE,
        .under_block = STONE,
        .h = 0.76f,
        .m = 0.5f,
        .t = 0.3f,
        .s = 0.9f,
        .e = 1.3f,
        .r = 1.4f,
        .y = 0.36f
    },
};

// h = height, [-1, 1]
// m = moisture, [0, 1]
// t = temperature [0, 1]
static enum Biome get_biome(f32 h, f32 m, f32 t) {
    if (h <= 0.0f) {
        return OCEAN;
    }

    const vec3s weight = (vec3s) {{ 1.0f, 1.0f, 1.0f }};

    vec3s b = (vec3s) {{ h, m, t }};
    enum Biome biome = OCEAN;
    f32 d = 1000.0f;

    for (size_t i = 0; i <= BIOME_LAST; i++) {
        vec3s a_hmt = b,
        b_hmt = (vec3s) {{
            BIOME_DATA[i].h,
            BIOME_DATA[i].m,
            BIOME_DATA[i].t
        }};

        for (size_t j = 0; j < 3; j++) {
            if (isnan(b_hmt.raw[j])) {
                a_hmt.raw[j] = 0;
                b_hmt.raw[j] = 0;
            }
        }

        f32 d_i = glms_vec3_norm(
            glms_vec3_mul(
                glms_vec3_sub(a_hmt, b_hmt),
                weight));

        if (d_i < d) {
            biome = i;
            d = d_i; 
        }
    }

    return biome;
}

static enum BlockId _get(struct Chunk *chunk, s32 x, s32 y, s32 z) {
    ivec3s p = (ivec3s) {{ x, y, z }};
    if (chunk_in_bounds(p)) {
        return chunk_get_block(chunk, p);
    } else {
        return world_get_block(chunk->world, glms_ivec3_add(chunk->position, p));
    }
}

static void _set(struct Chunk *chunk, s32 x, s32 y, s32 z, u32 d) {
    ivec3s p = (ivec3s){{x, y, z}};
    if (chunk_in_bounds(p)) {
        chunk_set_block(chunk, p, d);
    }
    
    ivec3s p_w = glms_ivec3_add(chunk->position, p);
    if (!world_contains(chunk->world, p_w)) {
        world_append_unloaded_block(chunk->world, p_w, d);
    } else {
        world_set_block(chunk->world, p_w, d);
    }
}


void worldgen_generate(struct Chunk *chunk) {
    SRAND(chunk->world->seed + ivec3shash(chunk->offset));

    struct Heightmap *heightmap = chunk_get_heightmap(chunk);

    // generate worldgen data if it doesn't exist for this chunk column yet
    if (!heightmap->flags.generated) {
        heightmap->flags.generated = true;

        struct Noise bs[] = {
            basic(1), basic(2),
            basic(3), basic(4)
        };

        struct Noise os[] = {
            octave(5, 0), octave(5, 1),
            octave(5, 2), octave(5, 3),
            octave(5, 4), octave(5, 5),
        };

        struct Noise cs[] = {
            combined(&bs[0], &os[1]),
            combined(&bs[1], &os[2]),
            combined(&os[3], &os[4])
        };

        struct Noise
            n_h = expscale(&os[0], 1.2f, 1.0f / 128.0f),
            n_m = expscale(&cs[0], 1.0f, 1.0f / 256.0f),
            n_t = expscale(&cs[1], 1.0f, 1.0f / 256.0f),
            n_r = cs[2];

        for (s64 x = 0; x < CHUNK_SIZE.x; x++) {
            for (s64 z = 0; z < CHUNK_SIZE.z; z++) {
                s64 wx = chunk->position.x + x, wz = chunk->position.z + z;

                f32 h = n_h.compute(&n_h.params, chunk->world->seed, wx, wz),
                    m = fabsf(n_m.compute(&n_m.params, chunk->world->seed, wx, wz)) * 1.2f,
                    t = fabsf(n_t.compute(&n_t.params, chunk->world->seed, wx, wz)) * 1.2f;

                enum Biome biome_id = get_biome(h, m, t);
                struct BiomeData biome = BIOME_DATA[biome_id];

                h = sign(h) * powf(fabsf(h), biome.e);

                f32 r;

                if (isnan(biome.r)) {
                    r = 0.0f;
                } else {
                    struct Noise n = expscale(&n_r, 1.1f, biome.r / 64.0f);
                    r = n.compute(&n.params, chunk->world->seed, wx, wz);
                }

                f32 s_y;
                if (isnan(biome.y)) {
                    s_y = 128.0f;
                } else {
                    s_y = 128.0f * (0.4f + (0.6f * biome.y));
                }

                heightmap->worldgen_data[x * CHUNK_SIZE.x + z] = (struct WorldgenData) {
                    .h_b = ((h * s_y) + (r * 16.0f)) * biome.s,
                    .b = biome_id
                };
            }
        }

#define WG_GET_H(_x, _z)\
    heightmap->worldgen_data[\
        clamp((_x), 0, CHUNK_SIZE.x - 1) * CHUNK_SIZE.x +\
        clamp((_z), 0, CHUNK_SIZE.z - 1)]

        // smooth heightmap
        for (s64 x = 0; x < CHUNK_SIZE.x; x++) {
            for (s64 z = 0; z < CHUNK_SIZE.z; z++) {
                f32 v = 0.0f;
                v += (WG_GET_H(x - 1, z - 1)).h_b;
                v += (WG_GET_H(x + 1, z - 1)).h_b;
                v += (WG_GET_H(x - 1, z + 1)).h_b;
                v += (WG_GET_H(x + 1, z + 1)).h_b;
                v *= 0.25f;
                WG_GET_H(x, z).h = v;
            }
        }
    }

    for (s64 x = 0; x < CHUNK_SIZE.x; x++) {
        for (s64 z = 0; z < CHUNK_SIZE.z; z++) {
            struct WorldgenData data = heightmap->worldgen_data[x * CHUNK_SIZE.x + z];
            s64 h = data.h;
            enum Biome biome = data.b;
            struct BiomeData biome_data = BIOME_DATA[biome]; 

            enum BlockId top_block = h > 48 ? SNOW : biome_data.top_block,
                under_block = biome_data.under_block;

            for (s64 y = 0; y < CHUNK_SIZE.y; y++) {
                s64 y_w = chunk->position.y + y;

                enum BlockId block = AIR;

                if (y_w > h && y_w <= WATER_LEVEL) {
                    block = WATER;
                } else if (y_w > h) {
                    continue;
                } else if (y_w == h) {
                    block = top_block;
                } else if (y_w >= (h - 3)) {
                    block = under_block;
                } else {
                    block = STONE;
                }

                chunk_set_block(chunk, (ivec3s) {{ x, y, z }}, block);

                if (y_w == h) {
                    // decorate
                    for (size_t i = 0; i < MAX_DECORATIONS; i++) {
                        if (biome_data.decorations[i].f == NULL) {
                            break;
                        }

                        if (RANDCHANCE(biome_data.decorations[i].chance)) {
                            biome_data.decorations[i].f(chunk, _get, _set, x, y, z);
                        }
                    }
                }
            }
        }
    }
}