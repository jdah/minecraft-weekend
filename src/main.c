#include "include/window.h"
#include "include/gfx.h"

#include "include/vao.h"
#include "include/vbo.h"
#include "include/shader.h"
#include "include/camera.h"
#include "include/state.h"
#include "include/block.h"
#include "include/sky.h"

// global state
struct State state;

void init() {
    block_init();
    state.window = &window;
    renderer_init(&state.renderer);
    world_init(&state.world);
    mouse_set_grabbed(true);

    state.world.player.camera.position = (vec3s) {{ 0, 80, 0 }};
}

void destroy() {
    renderer_destroy(&state.renderer);
    world_destroy(&state.world);
}

// TODO: remove this
#include "include/light.h"

void tick() {
    state.ticks++;
    world_tick(&state.world);
    world_set_center(&state.world, world_pos_to_block(state.world.player.camera.position));

    static ivec3s last_light;

    if (state.window->keyboard.keys[GLFW_KEY_C].pressed_tick) {
        last_light = world_pos_to_block(state.world.player.camera.position);
        u8 r = rand() % 16, g = rand() % 16, b = rand() % 16;
        light_add(&state.world, last_light, (r << 12) | (g << 8) | (b << 4) | 0xF);
    }

    if (state.window->keyboard.keys[GLFW_KEY_V].pressed_tick) {
        light_remove(&state.world, last_light);
    }
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