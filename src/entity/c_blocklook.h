#ifndef C_BLOCKLOOK_H
#define C_BLOCKLOOK_H

#include "../util/util.h"

struct BlockLookComponent {
    bool hit;
    ivec3s pos;
    enum Direction face;
    f32 radius;

    struct {
        bool render : 1;
    } flags;
};

#endif