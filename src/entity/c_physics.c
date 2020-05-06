#include "c_physics.h"
#include "ecs.h"
#include "../world/world.h"

#define GRAVITY ((vec3s) {{ 0, -0.005f, 0 }})

static void tick(struct PhysicsComponent *c_physics, struct Entity entity) {
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);

    if (c_physics->flags.gravity) {
        c_physics->velocity = glms_vec3_add(
            c_physics->velocity,
            GRAVITY);
    }

    c_position->position = glms_vec3_add(
        c_position->position,
        c_physics->velocity);
}

void c_physics_init(struct ECS *ecs) {
    ecs_register(C_PHYSICS, struct PhysicsComponent, ecs, ((union ECSSystem) {
        .init = NULL,
        .destroy = NULL,
        .render = NULL,
        .update = NULL,
        .tick = (ECSSubscriber) tick
    }));
}