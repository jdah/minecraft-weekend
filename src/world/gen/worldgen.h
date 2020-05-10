#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "../../util/util.h"

// forward declarations
struct Chunk;

typedef void (*FWGSet)(struct Chunk *, s32, s32, s32, u32);
typedef u32 (*FWGGet)(struct Chunk *, s32, s32, s32);
typedef void (*FWGDecorate)(struct Chunk  *, FWGGet, FWGSet, s64, s64, s64);

struct WorldgenData {
    f32 h_b;
    s64 h, b;
};

void worldgen_generate(struct Chunk *chunk);

void worldgen_pine(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z);
void worldgen_tree(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z);
void worldgen_flowers(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z);
void worldgen_grass(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z);
void worldgen_shrub(struct Chunk *chunk, FWGGet get, FWGSet set, s64 x, s64 y, s64 z);

#endif