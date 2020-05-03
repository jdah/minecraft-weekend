#ifndef DIRECTION_H
#define DIRECTION_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#pragma GCC diagnostic pop

enum Direction {
    NORTH = 0,
    SOUTH = 1,
    EAST = 2,
    WEST = 3,
    UP = 4,
    DOWN = 5
};

extern const ivec3s DIRECTION_IVEC[6];
extern const vec3s DIRECTION_VEC[6];

enum Direction _ivec3s2dir(ivec3s v);

#define DIR2VEC3S(d) (DIRECTION_VEC[d])
#define DIR2IVEC3S(d) (DIRECTION_IVEC[d])
#define IVEC3S2DIR(v) (_ivec3s2dir(v))


#endif