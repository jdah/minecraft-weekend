#ifndef POSITION_H
#define POSITION_H

#include "../util/util.h"

struct PositionComponent {
    vec3s position;

    ivec3s block, offset;
    bool block_changed, offset_changed;
};

#endif