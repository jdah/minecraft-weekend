#include "renderer.h"
#include "../state.h"

void renderer_init(struct Renderer *self) {
    *self = (struct Renderer) {0};
    self->camera_type = CAMERA_PERSPECTIVE;
    self->current_shader = SHADER_NONE;

    self->shaders[SHADER_BASIC_TEXTURE] = shader_create(
        "res/shaders/basic_texture.vs", "res/shaders/basic_texture.fs",
        2, (struct VertexAttr[]) {
            { .index = 0, .name = "position" },
            { .index = 1, .name = "uv" }
        });

    self->shaders[SHADER_CHUNK] = shader_create(
        "res/shaders/chunk.vs", "res/shaders/chunk.fs",
        3, (struct VertexAttr[]) {
            { .index = 0, .name = "position" },
            { .index = 1, .name = "uv" },
            { .index = 2, .name = "color" }
        });

    self->shaders[SHADER_SKY] = shader_create(
        "res/shaders/sky.vs", "res/shaders/sky.fs",
        2, (struct VertexAttr[]) {
            { .index = 0, .name = "position" },
            { .index = 1, .name = "uv" }
        });

    self->shaders[SHADER_BASIC_COLOR] = shader_create(
        "res/shaders/basic_color.vs", "res/shaders/basic_color.fs",
        1, (struct VertexAttr[]) {
            { .index = 0, .name = "position" }
        });


    self->block_atlas = blockatlas_create(
        "res/images/blocks.png",
        (ivec2s) {{ 16, 16 }}
    );

    self->textures[TEXTURE_CROSSHAIR] = texture_create_from_path("res/images/crosshair.png");
    self->textures[TEXTURE_CLOUDS] = texture_create_from_path("res/images/clouds.png"); 
    self->textures[TEXTURE_STAR] = texture_create_from_path("res/images/star.png");
    self->textures[TEXTURE_SUN] = texture_create_from_path("res/images/sun.png");
    self->textures[TEXTURE_MOON] = texture_create_from_path("res/images/moon.png");
    self->textures[TEXTURE_HOTBAR] = texture_create_from_path("res/images/hotbar.png");

    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, true);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);

    perspective_camera_init(&self->perspective_camera, radians(75.0f));
    ortho_camera_init(
        &self->ortho_camera, GLMS_VEC2_ZERO,
        (vec2s) {{ state.window->size.x, state.window->size.y }});
}

void renderer_destroy(struct Renderer *self) {
    for (size_t i = 0; i <= SHADERS_LAST; i++) {
        shader_destroy(self->shaders[i]);
    }

    blockatlas_destroy(&self->block_atlas);
    vao_destroy(self->vao);
    vbo_destroy(self->vbo);
    vbo_destroy(self->ibo);
}

void renderer_update(struct Renderer *self) {
    blockatlas_update(&self->block_atlas);
}

void renderer_prepare(struct Renderer *self, enum RenderPass pass) {
    switch (pass) {
        case PASS_2D:
            ortho_camera_init(
                &self->ortho_camera, GLMS_VEC2_ZERO,
                (vec2s) {{ state.window->size.x, state.window->size.y }});
            glClear(GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case PASS_3D:
            glClearColor(
                self->clear_color.x, self->clear_color.y,
                self->clear_color.z, self->clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glPolygonMode(GL_FRONT_AND_BACK, self->flags.wireframe ? GL_LINE : GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    shader_uniform_view_proj(self->shader, view_proj);
}

void renderer_use_shader(struct Renderer *self, enum ShaderType shader) {
    if (shader == self->current_shader) {
        return;
    }

    self->current_shader = shader;
    self->shader = self->shaders[shader];
    shader_bind(self->shader);
}

void renderer_quad_color(
    struct Renderer *self, vec2s size,
    vec4s color, mat4s model) {
    renderer_use_shader(self, SHADER_BASIC_COLOR);
    renderer_set_view_proj(self);
    shader_uniform_mat4(self->shader, "m", model);
    shader_uniform_vec4(self->shader, "color", color);

    vbo_buffer(self->vbo, (f32[]) {
        0, 0, 0,
        0, size.y, 0,
        size.x, size.y, 0,
        size.x, 0, 0,
    }, 0, (4 * 3) * sizeof(f32));

    vbo_buffer(self->ibo, (u32[]) {
        3, 0, 1, 3, 1, 2
    }, 0, 6 * sizeof(u32));

    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);

    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
}

void renderer_quad_texture(
    struct Renderer *self, struct Texture texture,
    vec2s size, vec4s color,
    vec2s uv_min, vec2s uv_max,
    mat4s model) {
    renderer_use_shader(self, SHADER_BASIC_TEXTURE);
    renderer_set_view_proj(self);
    shader_uniform_mat4(self->shader, "m", model);
    shader_uniform_texture2D(self->shader, "tex", texture, 0);
    shader_uniform_vec4(self->shader, "color", color);

    vbo_buffer(self->vbo, (f32[]) {
        0, 0, 0,
        0, size.y, 0,
        size.x, size.y, 0,
        size.x, 0, 0,

        uv_min.x, uv_min.y,
        uv_min.x, uv_max.y,
        uv_max.x, uv_max.y,
        uv_max.x, uv_min.y
    }, 0, ((4 * 3) + (4 * 2)) * sizeof(f32));

    vbo_buffer(self->ibo, (u32[]) {
        3, 0, 1, 3, 1, 2
    }, 0, 6 * sizeof(u32));

    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, 0, (4 * 3) * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
}

void renderer_aabb(
    struct Renderer *self, AABB aabb, vec4s color,
    mat4s model, enum FillMode fill_mode) {
    renderer_use_shader(self, SHADER_BASIC_COLOR);
    renderer_set_view_proj(self);
    shader_uniform_mat4(self->shader, "m", model);
    shader_uniform_vec4(self->shader, "color", color);

    u32 indices[] = {
        1, 0, 3, 1, 3, 2, // north (-z)
        4, 5, 6, 4, 6, 7, // south (+z)
        5, 1, 2, 5, 2, 6, // east (+x)
        0, 4, 7, 0, 7, 3, // west (-x)
        2, 3, 7, 2, 7, 6, // top (+y)
        5, 4, 0, 5, 0, 1  // bottom (-y)
    };

    vec3s min = aabb[0], max = aabb[1];
    f32 vertices[] = {
        min.x, min.y, min.z,
        max.x, min.y, min.z,
        max.x, max.y, min.z,
        min.x, max.y, min.z,

        min.x, min.y, max.z,
        max.x, min.y, max.z,
        max.x, max.y, max.z,
        min.x, max.y, max.z,
    };

    vbo_buffer(self->vbo, vertices, 0, (8 * 3) * sizeof(f32));
    vbo_buffer(self->ibo, indices, 0, 36 * sizeof(u32));

    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);

    vao_bind(self->vao);
    vbo_bind(self->ibo);

    if (fill_mode == FILL_MODE_LINE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *) 0);

    if (fill_mode == FILL_MODE_LINE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}