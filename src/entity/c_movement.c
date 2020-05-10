#include "c_movement.h"
#include "ecs.h"
#include "../world/world.h"

static void tick(struct MovementComponent *c_movement, struct Entity entity) {
    struct CameraComponent *c_camera = ecs_get(entity, C_CAMERA);
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);
    struct PhysicsComponent *c_physics = ecs_get(entity, C_PHYSICS);

    struct Block block = BLOCKS[world_get_block(entity.ecs->world, c_position->block)],
                 top_block = BLOCKS[world_get_block(
                     entity.ecs->world,
                     world_pos_to_block(glms_vec3_add(c_position->position,
                                                      (vec3s) {{ 0.0f, c_physics->size[1].y, 0.0f }})))];

    const f32 speed = 0.0245f * c_movement->speed * (c_movement->flags.flying ? 1.8f : 1.0f);

    vec3s movement, direction, forward, right;
    movement = GLMS_VEC3_ZERO;
    direction = GLMS_VEC3_ZERO;
    forward = (vec3s){{sinf(c_camera->camera.yaw), 0, cosf(c_camera->camera.yaw)}};
    right = glms_vec3_cross((vec3s){{0.0f, 1.0f, 0.0f}}, forward);

    if (c_movement->directions.forward) {
        direction = glms_vec3_add(direction, forward);
    }

    if (c_movement->directions.backward) {
        direction = glms_vec3_sub(direction, forward);
    }

    if (c_movement->directions.left) {
        direction = glms_vec3_add(direction, right);
    }

    if (c_movement->directions.right) {
        direction = glms_vec3_sub(direction, right);
    }

    if (c_movement->flags.flying) {
        if (c_movement->directions.up) {
            direction = glms_vec3_add(direction, (vec3s){{0.0f, 1.0f, 0.0f}});
        }

        if (c_movement->directions.down) {
            direction = glms_vec3_sub(direction, (vec3s){{0.0f, 1.0f, 0.0f}});
        }
    } else if (block.liquid) {
        // float up
        if (c_movement->directions.up) {
            // increase float speed to breach surface if the top of this entity is
            // above the liquid, even more so if the entity is also colliding on
            // the x or z axes (presumably trying to exit the liquid)
            const bool breaching = !top_block.liquid,
                exiting = breaching && (c_physics->stopped.x || c_physics->stopped.z);
            const f32 float_speed =
                speed * 0.7f *
                (breaching ? 1.4f : 1.0f) *
                (exiting ? 2.0f : 1.0f);

            c_physics->velocity.y += float_speed;
        }

        // swim down
        if (c_movement->directions.down) {
            c_physics->velocity.y -= speed * 0.7f;
        }
    } else if (c_movement->directions.up && c_physics->flags.grounded) {
        // jump
        c_physics->velocity.y += 0.16f * c_movement->jump_height;
    }

    if (isnan(glms_vec3_norm(direction))) {
        movement = GLMS_VEC3_ZERO;
    } else {
        movement = direction;
        movement = glms_vec3_normalize(movement);
        movement = glms_vec3_scale(movement, speed);
    }

    // determine if X/Z movement is slowed
    f32 xz_modifier;

    if (c_movement->flags.flying) {
        xz_modifier = 1.0f;
    } else if (block.liquid) {
        xz_modifier = c_physics->flags.grounded ? 0.8f : 0.45f;
    } else {
        xz_modifier = c_physics->flags.grounded ? 1.0f : 0.07f;
    }

    movement.x *= xz_modifier;
    movement.z *= xz_modifier;

    c_physics->flags.drag = !c_movement->flags.flying;
    c_physics->flags.gravity = !c_movement->flags.flying;

    if (c_movement->flags.flying) {
        c_physics->velocity = GLMS_VEC3_ZERO;

        const f32 flying_speed = 8.0f * c_movement->speed;
        physics_move(c_physics, entity, glms_vec3_scale(movement, flying_speed));
    } else {
        c_physics->velocity = glms_vec3_add(c_physics->velocity, movement);
    }
}

void c_movement_init(struct ECS *ecs) {
    ecs_register(C_MOVEMENT, struct MovementComponent, ecs, ((union ECSSystem) {
        .init = NULL,
        .destroy = NULL,
        .render = NULL,
        .update = NULL,
        .tick = (ECSSubscriber) tick
    }));
}