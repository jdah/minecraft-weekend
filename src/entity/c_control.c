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
    struct PhysicsComponent *c_physics = ecs_get(entity, C_PHYSICS);
    
    c_movement->directions.forward = state.window->keyboard.keys[GLFW_KEY_W] & BUTTON_STATE_DOWN;
    c_movement->directions.backward = state.window->keyboard.keys[GLFW_KEY_S] & BUTTON_STATE_DOWN;
    c_movement->directions.left = state.window->keyboard.keys[GLFW_KEY_A] & BUTTON_STATE_DOWN;
    c_movement->directions.right = state.window->keyboard.keys[GLFW_KEY_D] & BUTTON_STATE_DOWN;
    
    c_movement->directions.up = state.window->keyboard.keys[GLFW_KEY_SPACE] & BUTTON_STATE_DOWN;
    c_movement->directions.down = state.window->keyboard.keys[GLFW_KEY_LEFT_SHIFT] & BUTTON_STATE_DOWN;

    if (state.window->keyboard.keys[GLFW_KEY_K] & BUTTON_STATE_PRESSED_TICK) {
        c_movement->flags.flying = !c_movement->flags.flying;
    }

    if ((state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT] & BUTTON_STATE_PRESSED_TICK)
        && c_blocklook->hit) {
        world_set_block(entity.ecs->world, c_blocklook->pos, AIR);
    }

    struct Block held_block = BLOCKS[state.ui.hotbar.values[state.ui.hotbar.index]];

    if ((state.window->mouse.buttons[GLFW_MOUSE_BUTTON_RIGHT] & BUTTON_STATE_PRESSED_TICK)
        && c_blocklook->hit) {

        // only allow the block to be placed if it wouldn't collide with this player's AABB
        ivec3s pos_place = glms_ivec3_add(c_blocklook->pos, DIR2IVEC3S(c_blocklook->face));
        AABB aabb_place;
        held_block.get_aabb(entity.ecs->world, pos_place, aabb_place);

        if (!physics_collides(c_physics, aabb_place)) {
            world_set_block(entity.ecs->world, pos_place, held_block.id);
        }
    }

    struct LightComponent *c_light = ecs_get(entity, C_LIGHT);
    if (held_block.can_emit_light) {
        struct PositionComponent *c_position = ecs_get(entity, C_POSITION);

        c_light->flags.enabled = true;
        c_light->light = held_block.get_torchlight(entity.ecs->world, c_position->block);
    } else {
        c_light->flags.enabled = false;
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