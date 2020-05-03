#ifndef SKY_H
#define SKY_H

#include "gfx.h"
#include "util.h"
#include "vao.h"
#include "vbo.h"

// forward declarations
struct World;

enum CelestialBody {
    SUN, MOON
};

struct Sky {
    struct World *world;

    // minimum and maximum fog disstances
    f32 fog_near, fog_far;

    // fog and clear colors
    vec4s fog_color, clear_color;

    struct VBO ibo, vbo;
    struct VAO vao;
};

void sky_init(struct Sky *self, struct World *world);
void sky_destroy(struct Sky *self);
void sky_render(struct Sky *self);

#endif