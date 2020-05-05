#ifndef SKY_H
#define SKY_H

#include "../gfx/gfx.h"
#include "../util/util.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"

// forward declarations
struct World;

enum CelestialBody {
    SUN, MOON
};

enum SkyState {
    DAY = 0, NIGHT, SUNRISE, SUNSET
};

struct Sky {
    struct World *world;

    // minimum and maximum fog disstances
    f32 fog_near, fog_far;

    // fog and clear colors
    vec4s fog_color, clear_color;

    vec4s sunlight_color;

    enum SkyState state, state_day_night;
    f32 sky_state_progress, day_night_progress;

    struct VBO ibo, vbo;
    struct VAO vao;
};

void sky_init(struct Sky *self, struct World *world);
void sky_destroy(struct Sky *self);
void sky_render(struct Sky *self);

#endif