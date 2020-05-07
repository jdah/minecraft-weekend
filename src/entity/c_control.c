#include "c_control.h"

#include "../state.h"
#include "../ui/ui.h"

static void update(struct ControlComponent *c_control, struct Entity entity) {
    struct CameraComponent *c_camera = ecs_get(entity, C_CAMERA);

    c_camera->camera.pitch -= state.window->mouse.delta.y /
                              (((f32)state.window->frame_delta) /
                               (c_control->mouse_sensitivity * 10000.0f));

    c_camera->camera.yaw -= state.window->mouse.delta.x /
                            (((f32)state.window->frame_delta) /
                             (c_control->mouse_sensitivity * 10000.0f));
}

static void tick(struct ControlComponent *c_control, struct Entity entity) {
    struct MovementComponent *c_movement = ecs_get(entity, C_MOVEMENT);
    struct BlockLookComponent *c_blocklook = ecs_get(entity, C_BLOCKLOOK);
    
    c_movement->directions.forward = state.window->keyboard.keys[GLFW_KEY_W].down;
    c_movement->directions.backward = state.window->keyboard.keys[GLFW_KEY_S].down;
    c_movement->directions.left = state.window->keyboard.keys[GLFW_KEY_A].down;
    c_movement->directions.right = state.window->keyboard.keys[GLFW_KEY_D].down;
    
    c_movement->directions.up = state.window->keyboard.keys[GLFW_KEY_SPACE].down;
    c_movement->directions.down = state.window->keyboard.keys[GLFW_KEY_LEFT_SHIFT].down;

    if (state.window->keyboard.keys[GLFW_KEY_K].pressed_tick) {
        c_movement->flags.flying = !c_movement->flags.flying;
    }

    if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed_tick
        && c_blocklook->hit) {
        world_set_block(entity.ecs->world, c_blocklook->pos, AIR);
    }

    if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_RIGHT].pressed_tick
        && c_blocklook->hit) {
        world_set_block(
            entity.ecs->world,
            glms_ivec3_add(c_blocklook->pos, DIR2IVEC3S(c_blocklook->face)),
            state.ui.hotbar.values[state.ui.hotbar.index]);
    }
}

void c_control_init(struct ECS *ecs) {
    ecs_register(C_CONTROL, struct ControlComponent, ecs, ((union ECSSystem){
        .init = NULL,
        .destroy = NULL,
        .render = NULL,
        .update = (ECSSubscriber) update,
        .tick = (ECSSubscriber) tick
    }));
}