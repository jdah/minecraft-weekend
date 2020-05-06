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
    mouse_set_grabbed(true);

    struct Entity player = ecs_new(&state.world.ecs);
    ecs_add(player, C_POSITION);
    ecs_add(player, C_CAMERA);
    ecs_add(player, C_CONTROL);
    ecs_add(player, C_PHYSICS, ((struct PhysicsComponent) {
        .flags = {
            .gravity = true
        }
    }));

    struct ControlComponent *c_control = ecs_get(player, C_CONTROL);
    c_control->mouse_sensitivity = 3.0f;

    struct PositionComponent *c_position = ecs_get(player, C_POSITION);
    c_position->position = (vec3s) {{ 0, 80, 0}};

    state.world.entity_load = player;
    state.world.entity_view = player;
}

void destroy() {
    renderer_destroy(&state.renderer);
    world_destroy(&state.world);
}

void tick() {
    state.ticks++;
    world_tick(&state.world);

    // time warp
    if (state.window->keyboard.keys[GLFW_KEY_L].down) {
        state.world.ticks += 30;
    }

    if (state.window->keyboard.keys[GLFW_KEY_P].pressed_tick) {
        state.world.ticks += (TOTAL_DAY_TICKS) / 3;
    }

    static ivec3s last_light;

    // if (state.window->keyboard.keys[GLFW_KEY_C].pressed_tick) {
    //     last_light = world_pos_to_block(state.world.player.camera.position);
    //     srand(NOW());
    //     u8 r = rand() % 16, g = rand() % 16, b = rand() % 16;
    //     torchlight_add(&state.world, last_light, (r << 12) | (g << 8) | (b << 4) | 0xF);
    // }

    // if (state.window->keyboard.keys[GLFW_KEY_V].pressed_tick) {
    //     torchlight_remove(&state.world, last_light);
    // }


    // ivec3s p = world_pos_to_block(state.world.player.camera.position);
    // printf("%d\n", world_heightmap_get(&state.world, (ivec2s) {{ p.x, p.z }}));
}

void update() {
    renderer_update(&state.renderer);
    world_update(&state.world);

    // wireframe toggle (T)
    if (state.window->keyboard.keys[GLFW_KEY_T].pressed) {
        state.renderer.flags.wireframe = !state.renderer.flags.wireframe;
    }
}

void render() {
    renderer_prepare(&state.renderer, PASS_3D);
    world_render(&state.world);

    renderer_prepare(&state.renderer, PASS_2D);
    renderer_push_camera(&state.renderer);
    {
        renderer_set_camera(&state.renderer, CAMERA_ORTHO);
        renderer_immediate_quad(
            &state.renderer, state.renderer.textures[TEXTURE_CROSSHAIR],
            (vec3s) {{ (state.window->size.x / 2) - 8, (state.window->size.y / 2) - 8 }},
            (vec3s) {{ 16, 16 }}, (vec4s) {{ 1.0, 1.0, 1.0, 0.4}},
            (vec2s) {{ 0, 0 }}, (vec2s) {{ 1, 1 }});
    }
    renderer_pop_camera(&state.renderer);
}

int main(int argc, char *argv[]) {
    window_create(init, destroy, tick, update, render);
    window_loop();
}