#ifndef C_MOVEMENT_H
#define C_MOVEMENT_H

#include "../util/util.h"
#include "ecstypes.h"

struct MovementComponent {
    f32 speed;
    f32 jump_height;

    struct {
        bool forward: 1;
        bool backward: 1;
        bool left: 1;
        bool right: 1;
        bool up: 1;
        bool down: 1;
    } directions;

    struct {
        bool flying: 1;
    } flags;
};

#endif