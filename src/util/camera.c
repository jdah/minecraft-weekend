#include "camera.h"
#include "../gfx/window.h"

void perspective_camera_init(struct PerspectiveCamera *self, f32 fov) {
    memset(self, 0, sizeof(struct PerspectiveCamera));
    self->fov = fov;
    self->aspect = ((f32) window.size.x) / ((f32) window.size.y);
    self->znear = 0.01f;
    self->zfar = 1000.0f;
    perspective_camera_update(self);
}

void perspective_camera_update(struct PerspectiveCamera *self) {
    // bound pitch at +- pi/2 and yaw in [0, tau]
    self->pitch = clamp(self->pitch, -PI_2, PI_2);
    self->yaw = (self->yaw < 0 ? TAU : 0.0f) + fmodf(self->yaw, TAU);

    self->direction = (vec3s) {{
        cosf(self->pitch) * sinf(self->yaw),
        sinf(self->pitch),
        cosf(self->pitch) * cosf(self->yaw)
    }};
    glms_vec3_normalize(self->direction);

    self->right = glms_vec3_cross((vec3s) {{ 0.0f, 1.0f, 0.0f }}, self->direction);
    self->up = glms_vec3_cross(self->direction, self->right);

    self->view_proj.view = glms_lookat(self->position, glms_vec3_add(self->position, self->direction), self->up);
    self->view_proj.proj = glms_perspective(self->fov, self->aspect, self->znear, self->zfar);
}

void ortho_camera_init(struct OrthoCamera *self, vec2s min, vec2s max) {
    memset(self, 0, sizeof(struct OrthoCamera));
    self->min = min;
    self->max = max;
    ortho_camera_update(self);
}

void ortho_camera_update(struct OrthoCamera *self) {
    self->view_proj.view = glms_mat4_identity();
    // self->view_proj.view = glms_translate(self->view_proj.view, (vec3s) {{ -self->position.x, -self->position.y, 0 }});
    self->view_proj.proj = glms_ortho(self->min.x, self->max.x, self->min.y, self->max.y, -100.0f, 100.0f);
}