#ifndef C_PHYSICS_H
#define C_PHYSICS_H

#include "../util/util.h"
#include "ecstypes.h"

struct PhysicsComponent {
    // axes on which velocity has been stopped (there was a collision)
    ivec3s stopped;

    // current entity velocity
    vec3s velocity;

    // size AABB should have min (0, 0) and represent the size of the entity
    AABB size;

    // the curreent bounding box of this entity
    AABB aabb;

    struct {
        // if true, this object will collide with others
        bool collide: 1;

        // if true, gravity applies to this object
        bool gravity: 1;

        // if true, drag applies to this object
        bool drag: 1;

        // if true, this object is on the ground
        bool grounded: 1;
    } flags;
};

vec3s physics_move(
    struct PhysicsComponent *c_physics,
    struct Entity entity, vec3s movement);

bool physics_collides(struct PhysicsComponent *c_physics, AABB aabb);

#endif