#include "include/window.h"
#include "include/gfx.h"

#include "include/vao.h"
#include "include/vbo.h"
#include "include/shader.h"
#include "include/camera.h"
#include "include/state.h"
#include "include/block.h"

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

void tick() {
    world_tick(&state.world);
    world_set_center(&state.world, world_pos_to_block(state.world.player.camera.position));
    state.ticks++;
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
    state.renderer.clear_color = (vec4s) {{ 0.5f, 0.8f, 0.9f, 1.0f }};
    renderer_prepare(&state.renderer, PASS_3D);
    world_render(&state.world);
}

int main(int argc, char *argv[]) {
    window_create(init, destroy, tick, update, render);
    window_loop();
}