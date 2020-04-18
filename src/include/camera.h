#ifndef CAMERA_H
#define CAMERA_H

#include "gfx.h"
#include "util.h"

struct Camera {
    mat4s view, proj;
    vec3s position, direction, up, right;
    f32 pitch, yaw, fov, aspect, znear, zfar;
};

struct Camera camera_create(f32 fov);
void camera_update(struct Camera *self);

#endif