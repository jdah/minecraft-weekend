#ifndef C_PHYSICS_H
#define C_PHYSICS_H

#include "../util/util.h"

struct PhysicsComponent {
    vec3s velocity;
    vec3s aabb[2];

    struct {
        // if true, this object will collide with others
        bool collide: 1;

        // if true, gravity applies to this object
        bool gravity: 1;

        // if true, this object is on the ground
        bool grounded: 1;
    } flags;
};

#endif