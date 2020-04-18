#include "include/camera.h"
#include "include/window.h"

struct Camera camera_create(f32 fov) {
    struct Camera self;
    memset(&self, 0, sizeof(struct Camera));

    self.fov = fov;
    self.aspect = ((f32) window.size.x) / ((f32) window.size.y);
    self.znear = 0.01f;
    self.zfar = 1000.0f;

    camera_update(&self);
    return self;
}

void camera_update(struct Camera *self) {
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
    self->view = glms_mat4_identity();
    self->proj = glms_mat4_identity();

    self->view = glms_lookat(self->position, glms_vec3_add(self->position, self->direction), self->up);

    self->proj = glms_perspective(self->fov, self->aspect, self->znear, self->zfar);
}