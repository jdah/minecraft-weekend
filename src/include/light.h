#ifndef LIGHT_H
#define LIGHT_H

#include "util.h"
#include "world.h"

#define LIGHT_R(d) (u16) (((d) & 0xF000) >> 12)
#define LIGHT_G(d) (u16) (((d) & 0x0F00) >>  8)
#define LIGHT_B(d) (u16) (((d) & 0x00F0) >>  4)
#define LIGHT_I(d) (u16) (((d) & 0x000F) >>  0)

#define LIGHT_SET_R(d, r) (u16) (((d) & ~0xF000) | ((r) << 12))
#define LIGHT_SET_G(d, g) (u16) (((d) & ~0x0F00) | ((g) <<  8))
#define LIGHT_SET_B(d, b) (u16) (((d) & ~0x00F0) | ((b) <<  4))
#define LIGHT_SET_I(d, i) (u16) (((d) & ~0x000F) | ((i) <<  0))

void light_add(struct World *world, ivec3s pos, u16 light);
void light_remove(struct World *world, ivec3s pos);

#endif