#include "c_control.h"
#include "c_camera.h"
#include "c_position.h"

#include "../state.h"

static void init(struct ControlComponent *c_control, struct Entity entity) {
    
}

static void update(struct ControlComponent *c_control, struct Entity entity) {
    struct CameraComponent *c_camera = ecs_get(entity, C_CAMERA);

    c_camera->camera.pitch -= state.window->mouse.delta.y /
        (((f32) state.window->frame_delta) /
        (c_control->mouse_sensitivity * 10000.0f));

    c_camera->camera.yaw -= state.window->mouse.delta.x /
        (((f32) state.window->frame_delta) /
        (c_control->mouse_sensitivity * 10000.0f));
}

static void tick(struct ControlComponent *c_control, struct Entity entity) {
    struct CameraComponent *c_camera = ecs_get(entity, C_CAMERA);
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);

    const f32 speed = 0.22f;

    vec3s movement, direction, forward, right;
    movement = GLMS_VEC3_ZERO;
    direction = GLMS_VEC3_ZERO;
    forward = (vec3s) {{ sinf(c_camera->camera.yaw), 0, cosf(c_camera->camera.yaw) }};
    right = glms_vec3_cross((vec3s) {{ 0.0f, 1.0f, 0.0f }}, forward);

    if (state.window->keyboard.keys[GLFW_KEY_W].down) {
        direction = glms_vec3_add(direction, forward);
    }

    if (state.window->keyboard.keys[GLFW_KEY_S].down) {
        direction = glms_vec3_sub(direction, forward);
    }

    if (state.window->keyboard.keys[GLFW_KEY_A].down) {
        direction = glms_vec3_add(direction, right);
    }

    if (state.window->keyboard.keys[GLFW_KEY_D].down) {
        direction = glms_vec3_sub(direction, right);
    }

    if (state.window->keyboard.keys[GLFW_KEY_SPACE].down) {
        direction = glms_vec3_add(direction, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    }

    if (state.window->keyboard.keys[GLFW_KEY_LEFT_SHIFT].down) {
        direction = glms_vec3_sub(direction, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    }

    if (isnan(glms_vec3_norm(direction))) {
        movement = GLMS_VEC3_ZERO;
    } else {
        movement = direction;
        movement = glms_vec3_normalize(movement);
        movement = glms_vec3_scale(movement, speed);
    }

    c_position->position = glms_vec3_add(c_position->position, movement);
}

void c_control_init(struct ECS *ecs) {
    ecs_register(C_CONTROL, struct ControlComponent, ecs, ((union ECSSystem) {
        .init = (ECSSubscriber) init,
        .destroy = NULL,
        .render = NULL,
        .update = (ECSSubscriber) update,
        .tick = (ECSSubscriber) tick
    }));
}