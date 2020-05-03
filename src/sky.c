#include "include/sky.h"
#include "include/renderer.h"
#include "include/state.h"

// sun(rise/set) time in ticks
#define SUN_CHANGE_TICKS 100
#define HALF_SUN_CHANGE_TICKS (SUN_CHANGE_TICKS / 2)

enum SkyState {
    DAY, NIGHT, SUNRISE, SUNSET
};

enum SkyPlanes {
    SKY_PLANE, VOID_PLANE
};

// sky colors corresponding to enum values of SkyState
const vec4s SKY_COLORS[4] = {
    RGBAX2F(0x87CEEBFF),
    RGBAX2F(0x010422FF),
    RGBAX2F(0xFFCA7CFF),
    RGBAX2F(0xFFAB30FF)
};

// sky colors corresponding to [DAY or NIGHT][SKY_PLANE or VOID_PLANE]
const vec4s PLANE_COLORS[2][2] = {
    { RGBAX2F(0x87CEEBFF), RGBAX2F(0x87CEEBFF) },
    { RGBAX2F(0x030832FF), RGBAX2F(0x030832FF) }
};

void sky_init(struct Sky *self, struct World *world) {
    self->world = world;
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, false);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);

    // prevent a white flash on startup
    state.renderer.clear_color = SKY_COLORS[DAY];

    // place a unit quad of size 1.0 around (0, 0) in the VBO
    vbo_buffer(self->vbo, (f32[]) {
        // positions
        -0.5f, -0.5f, 0.0f,
        -0.5f, +0.5f, 0.0f,
        +0.5f, +0.5f, 0.0f,
        +0.5f, -0.5f, 0.0f,

        // UVs
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f    
    }, 0, ((3 * 4) + (2 * 4)) * sizeof(f32));

    vbo_buffer(self->ibo, (u32[]) {
        3, 0, 1, 3, 1, 2
    }, 0, 6 * sizeof(u32));
}

void sky_destroy(struct Sky *self) {
    vbo_destroy(self->vbo);
    vao_destroy(self->vao);
}

enum SkyState get_sky_state(u64 ticks) {
    const u64 day_ticks = (ticks % TOTAL_DAY_TICKS);

    if (day_ticks <= HALF_SUN_CHANGE_TICKS) {
        return SUNRISE;
    } else if (day_ticks <= (DAY_TICKS - HALF_SUN_CHANGE_TICKS)) {
        return DAY;
    } else if (day_ticks <= (DAY_TICKS + HALF_SUN_CHANGE_TICKS)) {
        return SUNSET;
    } else if (day_ticks <= TOTAL_DAY_TICKS - HALF_SUN_CHANGE_TICKS) {
        return NIGHT;
    } else {
        return SUNRISE;
    }
}

enum SkyState get_next_sky_state(enum SkyState state) {
    switch (state) {
        case DAY: return SUNSET;
        case NIGHT: return SUNRISE;
        case SUNRISE: return DAY;
        case SUNSET: return NIGHT;
    }
}

enum SkyState get_prev_sky_state(enum SkyState state) {
    switch (state) {
        case DAY: return SUNRISE;
        case NIGHT: return SUNSET;
        case SUNRISE: return NIGHT;
        case SUNSET: return DAY;
    }
}

// returns a value in [0, 1] indicating the "progress" of the current sky state
// (how far along it is in ticks)
f32 get_sky_state_progress(u64 ticks) {
    const u64 day_ticks = (ticks % TOTAL_DAY_TICKS);

    if (day_ticks <= HALF_SUN_CHANGE_TICKS) {
        return 0.5f + ((f32) day_ticks) / ((f32) HALF_SUN_CHANGE_TICKS);
    } else if (day_ticks <= (DAY_TICKS - HALF_SUN_CHANGE_TICKS)) {
        return ((f32) (day_ticks - HALF_SUN_CHANGE_TICKS)) / ((f32) DAY_TICKS - SUN_CHANGE_TICKS);
    } else if (day_ticks <= (DAY_TICKS + HALF_SUN_CHANGE_TICKS)) {
        return ((f32) (day_ticks - (DAY_TICKS - HALF_SUN_CHANGE_TICKS))) / ((f32) SUN_CHANGE_TICKS);
    } else if (day_ticks <= TOTAL_DAY_TICKS - HALF_SUN_CHANGE_TICKS) {
        return ((f32) (day_ticks - (DAY_TICKS + HALF_SUN_CHANGE_TICKS))) / ((f32) NIGHT_TICKS - SUN_CHANGE_TICKS);
    } else {
        return ((f32) (day_ticks - (TOTAL_DAY_TICKS - HALF_SUN_CHANGE_TICKS))) / ((f32) SUN_CHANGE_TICKS);
    }
}

enum SkyState get_day_night(u64 ticks) {
    const u64 day_ticks = (ticks % TOTAL_DAY_TICKS);
    return (day_ticks >= 0 && day_ticks <= DAY_TICKS) ? DAY : NIGHT;
}

// returns a value in [0, 1] indicating the "progress" of the current day or night
f32 get_day_night_progress(u64 ticks) {
    const u64 day_ticks = (ticks % TOTAL_DAY_TICKS);
    switch (get_day_night(ticks)) {
        case DAY:
            return ((f32) day_ticks) / ((f32) DAY_TICKS);
        default:
            return ((f32) (day_ticks - DAY_TICKS) / ((f32) NIGHT_TICKS));
    }
}

static mat4s plane_model(vec3s translation, vec3s rotation, vec3s scale) {
    mat4s m = glms_mat4_identity();
    m = glms_translate(m, translation);
    m = glms_rotate(m, rotation.x, (vec3s) {{ 1.0f, 0.0f, 0.0f }});
    m = glms_rotate(m, rotation.y, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    m = glms_rotate(m, rotation.z, (vec3s) {{ 0.0f, 0.0f, 1.0f }});
    m = glms_scale(m, scale);
    return m;
}

static mat4s celestial_model(enum CelestialBody body, vec3s center) {
    u64 day_ticks = (state.ticks % TOTAL_DAY_TICKS); 
    f32 time;
    bool show = true;

    switch (body) {
        case SUN:
            time = ((f32) day_ticks) / (f32) DAY_TICKS;
            show = day_ticks >= 0 && day_ticks <= DAY_TICKS;
            break;
        case MOON:
            time = (((f32) day_ticks) - DAY_TICKS) / (f32) NIGHT_TICKS;
            show = day_ticks > DAY_TICKS && day_ticks <= TOTAL_DAY_TICKS;
            break;
    }

    const f32 angle_start = -(PI + 0.4);
    const f32 angle_end = 0.4;
    const f32 angle = show ? fmodf(TAU + ((time * (angle_end - angle_start)) + angle_start), TAU) : -(PI + 1.0);

    mat4s m = glms_mat4_identity();
    m = glms_translate(m, glms_vec3_add(center, (vec3s) {{ 0.0f, 4.0f, 0.0f }}));
    m = glms_rotate(m, angle, (vec3s) {{ 1.0f, 0.0f, 0.0f }});
    m = glms_translate(m, (vec3s) {{ 0.0f, 0.0f, 10.0f }});
    m = glms_scale(m, (vec3s) {{ 8.0f, 8.0f, 0 }});

    return m;
}

// renders a sky plane with the specified transformation parameters
// assumes that the sky shader is already enabled and fog uniforms have been set
static void plane_render(struct Sky *self, struct Texture *tex, vec4s color, mat4s m) {
    shader_uniform_mat4(state.renderer.shaders[SHADER_SKY], "m", m);
    shader_uniform_vec4(state.renderer.shaders[SHADER_SKY], "color", color);

    shader_uniform_int(state.renderer.shaders[SHADER_SKY], "use_tex", tex != NULL);
    if (tex != NULL) {
        shader_uniform_texture2D(state.renderer.shaders[SHADER_SKY], "tex", *tex, 0);
    }

    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 0, 0);
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, 0, (3 * 4) * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
}

void sky_render(struct Sky *self) {
    vec3s center = state.renderer.perspective_camera.position;

    // disable depth writing so the sky is always overwritten by everything else
    glDepthMask(GL_FALSE);

    const f32 t = get_sky_state_progress(state.ticks);
    vec4s fog_color, sky_color, void_color;

    enum SkyState sky_state = get_sky_state(state.ticks);
    switch (sky_state) {
        case DAY:
            fog_color = SKY_COLORS[DAY];
            sky_color = SKY_COLORS[DAY];
            void_color = SKY_COLORS[DAY];
            break;
        case NIGHT:
            fog_color = SKY_COLORS[NIGHT];
            sky_color = SKY_COLORS[NIGHT];
            void_color = SKY_COLORS[NIGHT];
            break;
        case SUNRISE:
            fog_color = rgba_lerp3(
                SKY_COLORS[NIGHT],
                SKY_COLORS[SUNRISE],
                SKY_COLORS[DAY],
                t);
            sky_color = rgba_lerp(
                PLANE_COLORS[NIGHT][SKY_PLANE],
                PLANE_COLORS[DAY][SKY_PLANE],
                t);
            void_color = rgba_lerp3(
                PLANE_COLORS[DAY][SKY_PLANE],
                SKY_COLORS[SUNRISE],
                PLANE_COLORS[DAY][SKY_PLANE],
                t);
            break;
        case SUNSET:
            fog_color = rgba_lerp3(
                SKY_COLORS[DAY],
                SKY_COLORS[SUNSET],
                SKY_COLORS[NIGHT],
                t);
            sky_color = rgba_lerp(
                PLANE_COLORS[DAY][SKY_PLANE],
                PLANE_COLORS[NIGHT][SKY_PLANE],
                t);
            void_color = rgba_lerp3(
                PLANE_COLORS[DAY][VOID_PLANE],
                SKY_COLORS[SUNSET],
                PLANE_COLORS[NIGHT][VOID_PLANE],
                t);
            break;
    }

    self->fog_color = fog_color;
    self->clear_color = self->fog_color;

    renderer_use_shader(&state.renderer, SHADER_SKY);
    renderer_set_view_proj(&state.renderer);
    shader_uniform_vec4(state.renderer.shaders[SHADER_SKY], "fog_color", self->fog_color);
    shader_uniform_float(state.renderer.shaders[SHADER_SKY], "fog_near", self->fog_near / 8.0f);
    shader_uniform_float(state.renderer.shaders[SHADER_SKY], "fog_far", self->fog_far);

    // sky plane
    plane_render(
        self, NULL,
        sky_color,
        plane_model(
            glms_vec3_add(center, (vec3s) {{ 0.0f, 16.0f, 0.0f }}),
            (vec3s) {{ radians(-90.0f), 0.0f, 0.0f }},
            (vec3s) {{ 1024.0f, 1024.0f, 0 }}));

    // void plane
    plane_render(
        self, NULL,
        void_color,
        plane_model(
            glms_vec3_add(center, (vec3s) {{ 0.0f, -16.0f, 0.0f }}),
            (vec3s) {{ radians(90.0f), 0.0f, 0.0f }},
            (vec3s) {{ 1024.0f, 1024.0f, 0 }}));

    const f32 time = ((f32) (state.ticks % 600)) / 600.0f;
    const f32 angle = time * TAU;

    // sun
    plane_render(
        self, &state.renderer.textures[TEXTURE_SUN],
        (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
        celestial_model(SUN, center));

    // moon
    plane_render(
        self, &state.renderer.textures[TEXTURE_MOON],
        (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
        celestial_model(MOON, center));

    glDepthMask(GL_TRUE);
}