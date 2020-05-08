#include "c_physics.h"
#include "ecs.h"
#include "../world/world.h"

const f32 MOVEMENT_EPSILON = 0.01f;

const vec3s GRAVITY = (vec3s) {{ 0, -0.0086f, 0 }};

static f32 move_axis(AABB aabb, f32 movement, vec3s axis, AABB *colliders, size_t size) {
    vec3s d_v = glms_vec3_scale(axis, movement);
    f32 m_sign = sign(movement);

    size_t axis_index = 0;
    for (size_t i = 0; i < 3; i++) {
        if (axis.raw[i] != 0.0f) {
            axis_index = i;
            break;
        }
    }

    AABB moved;
    glms_aabb_transform(aabb, glms_translate_make(d_v), moved);

    for (size_t i = 0; i < size; i++) {
        if (glms_aabb_aabb(moved, colliders[i])) {
            // compute collision depth, resolve, and re-translate initial AABB
            f32 depth = glms_aabb_aabb_depth(moved, colliders[i]).raw[axis_index];
            d_v.raw[axis_index] += -m_sign * (depth + MOVEMENT_EPSILON);
            glms_aabb_transform(aabb, glms_translate_make(d_v), moved);

            // zero movement if it was effectively stopped
            if (fabsf(d_v.raw[axis_index]) <= MOVEMENT_EPSILON) {
                d_v.raw[axis_index] = 0.0f;
                break;
            }
        }
    }

    f32 result = d_v.raw[axis_index];
    return fabsf(result) <= F32_EPSILON ? 0.0f : result; 
}

static vec3s move(AABB aabb, vec3s movement, AABB *colliders, size_t size) {
    vec3s result;
    AABB current = AABB_COPY(aabb);

    // move one axis at a time, translating in between to ensure consistency
    for (size_t i = 0; i < 3; i++) {
        vec3s axis = GLMS_VEC3_ZERO;
        axis.raw[i] = 1.0f;

        f32 movement_axis = move_axis(current, movement.raw[i], axis, colliders, size);
        glms_aabb_transform(
            current,
            glms_translate_make(glms_vec3_scale(axis, movement_axis)),
            current);
        result.raw[i] = movement_axis;
    }

    return result;
}

// creates an AABB of the phyics component's size centered around its position
static void make_aabb(
    struct PhysicsComponent *c_physics,
    struct PositionComponent *c_position,
    AABB dest) {
    vec3s half_size = glms_vec3_divs(glms_vec3_sub(c_physics->size[1], c_physics->size[0]), 2.0f);
    glms_aabb_transform(
        c_physics->size,
        glms_translate_make(glms_vec3_sub(c_position->position, half_size)),
        dest);
}

static void tick(struct PhysicsComponent *c_physics, struct Entity entity) {
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);
    struct Block block = BLOCKS[world_get_block(entity.ecs->world, c_position->block)];

    if (c_physics->flags.gravity) {
        c_physics->velocity = glms_vec3_add(
            c_physics->velocity,
            glms_vec3_scale(GRAVITY, block.solid ? 1.0f : block.gravity_modifier));
    }

    // attempt to move by velocity
    vec3s moved = physics_move(c_physics, entity, c_physics->velocity);

    // zero velocity on axes which did not move completely
    for (size_t i = 0; i < 3; i++) {
        bool stopped = fabsf(moved.raw[i] - c_physics->velocity.raw[i]) >= F32_EPSILON;
        c_physics->stopped.raw[i] = stopped;
        c_physics->velocity.raw[i] *= stopped ? 0.0f : 1.0f; 
    }

    // keep track of whether or not this entity is on the ground
    c_physics->flags.grounded = c_physics->velocity.y <= 0 && c_physics->stopped.y;

    // simulate drag
    if (c_physics->flags.drag) {
        c_physics->velocity = glms_vec3_add(
            c_physics->velocity,
            glms_vec3_scale(c_physics->velocity, -0.02f * block.drag));
    }

    if (c_physics->flags.grounded) {
        const f32 sliperiness = block.sliperiness * 0.6f;
        c_physics->velocity.x *= sliperiness;
        c_physics->velocity.z *= sliperiness;
    }
}

// attempt to move a physics component belonging to the specified entity
vec3s physics_move(
    struct PhysicsComponent *c_physics,
    struct Entity entity, vec3s movement) {
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);

    if (!c_physics->flags.collide) {
        c_position->position = glms_vec3_add(c_position->position, movement);
        make_aabb(c_physics, c_position, c_physics->aabb);
        return movement;
    }

    // ensure AABB is updated    
    make_aabb(c_physics, c_position, c_physics->aabb);

    // get AABBs in a scaled area
    AABB area = AABB_COPY(c_physics->aabb);
    glms_aabb_scale(area, (vec3s) {{ 2, 2, 2 }}, area);

    AABB aabbs[256];
    size_t n = world_get_aabbs(entity.ecs->world, area, aabbs, 256);

    vec3s moved = move(c_physics->aabb, movement, aabbs, n);
    c_position->position = glms_vec3_add(
        c_position->position,
        moved
    );

    // update AABB according to position change
    make_aabb(c_physics, c_position, c_physics->aabb);

    return moved;
}

// returns true if the physics component collides with the specified AABB
bool physics_collides(struct PhysicsComponent *c_physics, AABB aabb) {
    return glms_aabb_aabb(c_physics->aabb, aabb);
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