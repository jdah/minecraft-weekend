#include "gfx/window.h"
#include "gfx/gfx.h"

#include "state.h"
#include "block/block.h"

// TODO: remove these
#include "world/light.h"

// global state
struct State state;

void init() {
    block_init();
    state.window = &window;
    renderer_init(&state.renderer);
    world_init(&state.world);
    ui_init(&state.ui);
    mouse_set_grabbed(true);

    struct Entity player = ecs_new(&state.world.ecs);
    ecs_add(player, C_POSITION);
    ecs_add(player, C_PHYSICS, ((struct PhysicsComponent) {
        .size = {
            (vec3s) {{ 0, 0, 0 }},
            (vec3s) {{ 0.2f, 1.6f, 0.2f }}
        },
        .flags = {
            .gravity = true,
            .collide = true
        }
    }));
    ecs_add(player, C_MOVEMENT, ((struct MovementComponent) {
        .speed = 1.0f,
        .jump_height = 1.0f
    }));
    ecs_add(player, C_CAMERA, ((struct CameraComponent) {
        .offset = (vec3s) {{ 0.0f, 0.8f, 0.0f }}
    }));
    ecs_add(player, C_CONTROL);
    ecs_add(player, C_BLOCKLOOK, ((struct BlockLookComponent) {
        .radius = 5.0f,
        .flags = {
            .render = true
        }
    }));
    ecs_add(player, C_LIGHT);

    struct ControlComponent *c_control = ecs_get(player, C_CONTROL);
    c_control->mouse_sensitivity = 3.0f;

    struct PositionComponent *c_position = ecs_get(player, C_POSITION);
    c_position->position = (vec3s) {{ 0, 80, 0 }};

    state.world.entity_load = player;
    state.world.entity_view = player;
}

void destroy() {
    renderer_destroy(&state.renderer);
    world_destroy(&state.world);
    ui_destroy(&state.ui);
}

void tick() {
    state.ticks++;
    world_tick(&state.world);
    ui_tick(&state.ui);

    // time warp
    if (state.window->keyboard.keys[GLFW_KEY_L].down) {
        state.world.ticks += 30;
    }

    if (state.window->keyboard.keys[GLFW_KEY_P].pressed_tick) {
        state.world.ticks += (TOTAL_DAY_TICKS) / 3;
    }
}

void update() {
    renderer_update(&state.renderer);
    world_update(&state.world);
    ui_update(&state.ui);

    // wireframe toggle (T)
    if (state.window->keyboard.keys[GLFW_KEY_T].pressed) {
        state.renderer.flags.wireframe = !state.renderer.flags.wireframe;
    }

    // mouse toggle (ESC)
    if (state.window->keyboard.keys[GLFW_KEY_ESCAPE].pressed) {
        mouse_set_grabbed(!mouse_get_grabbed());
    }
}

void render() {
    renderer_prepare(&state.renderer, PASS_3D);
    world_render(&state.world);

    renderer_prepare(&state.renderer, PASS_2D);
    renderer_push_camera(&state.renderer);
    {
        renderer_set_camera(&state.renderer, CAMERA_ORTHO);
        ui_render(&state.ui);
    }
    renderer_pop_camera(&state.renderer);
}

int main(int argc, char *argv[]) {
    window_create(init, destroy, tick, update, render);
    window_loop();
}