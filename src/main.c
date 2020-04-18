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
    state.shader = shader_create(
        "res/shaders/basic.vs", "res/shaders/basic.fs",
        1, (struct VertexAttr[]) {
            { .index = 0, .name = "position" },
            { .index = 1, .name = "uv" },
            { .index = 2, .name = "color" }
        });
    state.atlas = atlas_create("res/images/blocks.png", (ivec2s) {{ 16, 16 }});
    world_init(&state.world);
    state.wireframe = false;
    mouse_set_grabbed(true);

    // OpenGL configuration
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    state.world.player.camera.position = (vec3s) {{ 0, 80, 0 }};
}

void destroy() {
    shader_destroy(state.shader);
    world_destroy(&state.world);
}

void tick() {
    world_tick(&state.world);

    // Load chunks around the current camera position
    world_set_center(&state.world, world_pos_to_block(state.world.player.camera.position));
}

void update() {
    world_update(&state.world);

    // wireframe toggle (T)
    if (state.window->keyboard.keys[GLFW_KEY_T].pressed) {
        state.wireframe = !state.wireframe;
    }
}

void render() {
    glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, state.wireframe ? GL_LINE : GL_FILL);
    world_render(&state.world);
}

int main(int argc, char *argv[]) {
    window_create(init, destroy, tick, update, render);
    window_loop();
}