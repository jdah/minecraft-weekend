#ifndef LIGHT_H
#define LIGHT_H

#include "../util/util.h"

// forward declarations
struct Chunk;
struct World;

#define LIGHT_MAX 15

typedef u16 Torchlight;
typedef u8 Sunlight;
typedef u32 Light;

#define TORCHLIGHT_R(d) (Torchlight) (((d) & 0xF000) >> 12)
#define TORCHLIGHT_G(d) (Torchlight) (((d) & 0x0F00) >>  8)
#define TORCHLIGHT_B(d) (Torchlight) (((d) & 0x00F0) >>  4)
#define TORCHLIGHT_I(d) (Torchlight) (((d) & 0x000F) >>  0)

#define TORCHLIGHT_SET_R(d, r) (Torchlight) (((d) & ~0xF000) | ((r) << 12))
#define TORCHLIGHT_SET_G(d, g) (Torchlight) (((d) & ~0x0F00) | ((g) <<  8))
#define TORCHLIGHT_SET_B(d, b) (Torchlight) (((d) & ~0x00F0) | ((b) <<  4))
#define TORCHLIGHT_SET_I(d, i) (Torchlight) (((d) & ~0x000F) | ((i) <<  0))

#define TORCHLIGHT_OF(r, g, b, i) (\
    (((Torchlight) (r)) << 12) |\
    (((Torchlight) (g)) <<  8) |\
    (((Torchlight) (b)) <<  4) |\
    (((Torchlight) (i)) <<  0))

#define LIGHT_OF(_sun, _torch) ((((u32) (_sun)) << 16) | ((u32) (_torch)))

void torchlight_add(struct World *world, ivec3s pos, Torchlight light);
void torchlight_remove(struct World *world, ivec3s pos);

void light_remove(struct World *world, ivec3s pos);
void light_apply(struct Chunk *chunk);
void light_update(struct World *world, ivec3s pos);

#endif