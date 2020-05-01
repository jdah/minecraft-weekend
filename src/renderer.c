#include "include/renderer.h"
#include "include/state.h"

void renderer_init(struct Renderer *self) {
    *self = (struct Renderer) {0};
    self->camera_type = CAMERA_PERSPECTIVE;
    self->current_shader = SHADER_NONE;

    self->shaders[SHADER_BASIC_2D] = shader_create(
        "res/shaders/basic2d.vs", "res/shaders/basic2d.fs",
        3, (struct VertexAttr[]) {
            { .index = 0, .name = "position" },
            { .index = 1, .name = "uv" },
            { .index = 2, .name = "color" }
        });
    self->shaders[SHADER_CHUNK] = shader_create(
        "res/shaders/chunk.vs", "res/shaders/chunk.fs",
        3, (struct VertexAttr[]) {
            { .index = 0, .name = "position" },
            { .index = 1, .name = "uv" },
            { .index = 2, .name = "color" }
        });

    self->block_atlas = blockatlas_create(
        "res/images/blocks.png",
        (ivec2s) {{ 16, 16 }}
    );

    perspective_camera_init(&self->perspective_camera, radians(75.0f));
    ortho_camera_init(
        &self->ortho_camera, GLMS_VEC2_ZERO,
        (vec2s) {{ state.window->size.x, state.window->size.y }});

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void renderer_destroy(struct Renderer *self) {
    for (size_t i = 0; i <= SHADERS_LAST; i++) {
        shader_destroy(self->shaders[i]);
    }

    blockatlas_destroy(&self->block_atlas);
}

void renderer_update(struct Renderer *self) {
    blockatlas_update(&self->block_atlas);
}

void renderer_prepare(struct Renderer *self, enum RenderPass pass) {
    switch (pass) {
        case PASS_2D:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case PASS_3D:
            glClearColor(
                self->clear_color.x, self->clear_color.y,
                self->clear_color.z, self->clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, self->flags.wireframe ? GL_LINE : GL_FILL);
            break;
    }
}

void renderer_set_camera(struct Renderer *self, enum CameraType type) {
    self->camera_type = type;
}

void renderer_push_camera(struct Renderer *self) {
    assert(self->camera_stack.size + 1 < CAMERA_STACK_MAX);
    self->camera_stack.array[self->camera_stack.size++] = self->camera_type;
}

void renderer_pop_camera(struct Renderer *self) {
    assert(self->camera_stack.size != 0);
    self->camera_type = self->camera_stack.array[--self->camera_stack.size];
}

void renderer_set_view_proj(struct Renderer *self) {
    struct ViewProj view_proj;

    switch (self->camera_type) {
        case CAMERA_PERSPECTIVE:
            view_proj = self->perspective_camera.view_proj;
            break;
        case CAMERA_ORTHO:
            view_proj = self->ortho_camera.view_proj;
            break;
    }

    shader_uniform_view_proj(
        self->shaders[self->current_shader],
        view_proj);
}

void renderer_use_shader(struct Renderer *self, enum ShaderType shader) {
    if (shader == self->current_shader) {
        return;
    }
    self->current_shader = shader;
    shader_bind(self->shaders[shader]);
}