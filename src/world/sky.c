#include "../gfx/renderer.h"
#include "../state.h"
#include "sky.h"

// sun(rise/set) time in ticks
#define SUN_CHANGE_TICKS (45 * TICKRATE)
#define HALF_SUN_CHANGE_TICKS (SUN_CHANGE_TICKS / 2)

enum SkyPlanes {
    SKY_PLANE = 0, FOG_PLANE, VOID_PLANE
};

// sky colors corresponding to [SkyState][SkyPlane]
const vec4s SKY_COLORS[4][3] = {
    { RGBAX2F(0x87CEEBFF), RGBAX2F(0x87CEEBFF), RGBAX2F(0x87CEEBFF) }, // DAY
    { RGBAX2F(0x020206FF), RGBAX2F(0x010104FF), RGBAX2F(0x000000FF) }, // NIGHT
    { RGBAX2F(0xFFCA7CFF), RGBAX2F(0xFFCA7CFF), RGBAX2F(0x000000FF) }, // SUNRISE
    { RGBAX2F(0xFFAB30FF), RGBAX2F(0xFFAB30FF), RGBAX2F(0x000000FF) }  // SUNSET
};

// sunlight colors corresponding to [DAY or NIGHT]
const vec4s SUNLIGHT_COLORS[2] = {
    RGBAX2F(0xFFFFFFFF),
    RGBAX2F(0x000000FF)
};

// cloud colors corresponding to [DAY or NIGHT]
const vec4s CLOUD_COLORS[2] = {
    RGBAX2F(0xFFFFFFFF),
    RGBAX2F(0x040404FF)
};

void sky_init(struct Sky *self, struct World *world) {
    self->world = world;
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, false);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);

    // prevent a white flash on startup
    state.renderer.clear_color = SKY_COLORS[DAY][FOG_PLANE];

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

enum SkyState get_sky_state(struct Sky *self) {
    const u64 day_ticks = (self->world->ticks % TOTAL_DAY_TICKS);

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

// returns a value in [0, 1] indicating the "progress" of the current sky state
// (how far along it is in ticks)
f32 get_sky_state_progress(struct Sky *self) {
    const f32 day_ticks = (f32) (self->world->ticks % TOTAL_DAY_TICKS);

    switch (self->state) {
        case SUNRISE:
            return day_ticks <= HALF_SUN_CHANGE_TICKS ?
                (0.5f + ((day_ticks / HALF_SUN_CHANGE_TICKS) * 0.5f)) :
                (((day_ticks - (TOTAL_DAY_TICKS - HALF_SUN_CHANGE_TICKS)) / HALF_SUN_CHANGE_TICKS) * 0.5f);
        case DAY:
            return (day_ticks - HALF_SUN_CHANGE_TICKS) / DAY_TICKS;
        case SUNSET:
            return (day_ticks - (DAY_TICKS - HALF_SUN_CHANGE_TICKS)) / SUN_CHANGE_TICKS;
        case NIGHT:
            return (day_ticks - (DAY_TICKS + HALF_SUN_CHANGE_TICKS)) / NIGHT_TICKS;
    }
}

enum SkyState get_day_night(struct Sky *self) {
    const u64 day_ticks = (self->world->ticks % TOTAL_DAY_TICKS);
    return (day_ticks >= 0 && day_ticks <= DAY_TICKS) ? DAY : NIGHT;
}

// returns a value in [0, 1] indicating the "progress" of the current day or night
f32 get_day_night_progress(struct Sky *self) {
    const u64 day_ticks = (self->world->ticks % TOTAL_DAY_TICKS);
    switch (get_day_night(self)) {
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

static mat4s celestial_model(struct Sky *self, enum CelestialBody body, vec3s center) {    
    const bool show = self->state_day_night == (body == SUN ? DAY : NIGHT);

    const f32
        base_start = -(PI + 0.5f),
        base_end = 0.5f;

    const struct {
        struct {
            f32 start, end;
        } show, hide;
    } angles = {
        .show = { base_start, base_end },
        .hide = { base_end, fmodf(TAU + base_start, TAU) }
    };

    const f32
        start = show ? angles.show.start : angles.hide.start,
        end = show ? angles.show.end : angles.hide.end,
        angle = fmodf(TAU + (self->day_night_progress * (end - start)) + start, TAU);

    mat4s m = glms_mat4_identity();
    m = glms_translate(m, glms_vec3_add(center, (vec3s) {{ 0.0f, 4.0f, 0.0f }}));
    m = glms_rotate(m, radians(-90.0f), (vec3s) {{ 0.0f, 1.0f, 0.0f }});
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

// render stars
static void stars_render(struct Sky *self, vec3s center, struct Texture tex) {
    if (self->state == DAY) {
        return;
    }

    SRAND(0x57A125);

    // compuet alpha: 1.0 during night, fade out/in during sunrise/sunset
    const f32 a = self->state == NIGHT ? 1.0f :
            (self->state == SUNRISE ?
                lerpf(1.0f, 0.0f, self->sky_state_progress) :
                lerpf(0.0f, 1.0f, self->sky_state_progress));

    const f32 angle = (((f32) (self->world->ticks)) / TOTAL_DAY_TICKS) * TAU;

    // TODO: do this via instanced rendering
    for (size_t i = 0; i < 512; i++) {
        const f32 angle_offset = radians((f32) RAND(0, 360)),
            scale_offset = RAND(-1, 6) * 0.05f,
            x_offset = RAND(-48, 48),
            z_offset = RAND(-48, 48),
            y_offset = RAND(-48, 48),
            r_offset = RAND(-15, 0) / 100.0f,
            g_offset = RAND(-15, 0) / 100.0f,
            b_offset = RAND(-15, 0) / 100.0f;
        
        const u64 twinkle_rate = RAND(TICKRATE, TICKRATE * 2);
        const f32
            twinkle = RANDCHANCE(0.08) ?
                (((f32) ((self->world->ticks + RAND(0, 100)) % twinkle_rate) / (f32) twinkle_rate) - 0.5f) * 0.25f :
                0.0f;

        mat4s m = glms_mat4_identity();
        m = glms_translate(m, glms_vec3_add(center, (vec3s) {{ 0.0f, 0.0f, 0.0f }}));
        m = glms_rotate(m, radians(-90.0f), (vec3s) {{ 0.0f, 1.0f, 0.0f }});
        m = glms_rotate(m, angle + angle_offset, (vec3s) {{ 1.0f, -0.3f, 0.25f }});
        m = glms_translate(m, (vec3s) {{ x_offset, y_offset, z_offset }});
        m = glms_scale(m, (vec3s) {{ 0.4f + scale_offset, 0.4f + scale_offset, 0 }});

        plane_render(
            self, &tex,
            (vec4s) {{
                1.0f + r_offset,
                1.0f + g_offset,
                1.0f + b_offset,
                a + twinkle
            }}, m);
    }
}

// get colors for DAY or NIGHT
static void colors_day_night(
    enum SkyState state, vec4s *sun, vec4s *fog,
    vec4s *sky, vec4s *void_, vec4s *cloud) {
    *sun = SUNLIGHT_COLORS[state];
    *fog = SKY_COLORS[state][FOG_PLANE];
    *sky = SKY_COLORS[state][SKY_PLANE];
    *void_ = SKY_COLORS[state][VOID_PLANE];
    *cloud = CLOUD_COLORS[state];
}

// get colors for a transition period, SUNRISE or SUNSET
static void colors_transition(
    struct Sky *self, enum SkyState state, enum SkyState from, enum SkyState to,
    vec4s *sun, vec4s *fog, vec4s *sky, vec4s *void_, vec4s *cloud) {
    *sun = rgba_lerp(
        SUNLIGHT_COLORS[from],
        SUNLIGHT_COLORS[to],
        self->sky_state_progress);
    *fog = rgba_lerp3(
        SKY_COLORS[from][FOG_PLANE],
        SKY_COLORS[state][FOG_PLANE],
        SKY_COLORS[to][FOG_PLANE],
        self->sky_state_progress);
    *sky = rgba_lerp(
        SKY_COLORS[from][SKY_PLANE],
        SKY_COLORS[to][SKY_PLANE],
        self->sky_state_progress);
    *void_ = rgba_lerp(
        SKY_COLORS[from][VOID_PLANE],
        SKY_COLORS[to][VOID_PLANE],
        self->sky_state_progress);
    *cloud = rgba_lerp(
        CLOUD_COLORS[from],
        CLOUD_COLORS[to],
        self->sky_state_progress);
}

void sky_render(struct Sky *self) {
    // update sky values
    self->state = get_sky_state(self);
    self->state_day_night = get_day_night(self);
    self->sky_state_progress = get_sky_state_progress(self);
    self->day_night_progress = get_day_night_progress(self);

    vec3s center = state.renderer.perspective_camera.position;

    // disable depth writing so the sky is always overwritten by everything else
    glDepthMask(GL_FALSE);

    vec4s fog_color, sky_color, void_color, cloud_color;

    switch (self->state) {
        case DAY:
        case NIGHT:
            colors_day_night(
                self->state, &self->sunlight_color,
                &fog_color, &sky_color, &void_color, &cloud_color);
            break;
        case SUNRISE:
        case SUNSET:
            colors_transition(
                self, self->state, self->state == SUNSET ? DAY : NIGHT, 
                self->state == SUNSET ? NIGHT : DAY, &self->sunlight_color,
                &fog_color, &sky_color, &void_color,  &cloud_color);
            break;
    }

    self->fog_color = fog_color;
    self->clear_color = self->fog_color;

    renderer_use_shader(&state.renderer, SHADER_SKY);
    renderer_set_view_proj(&state.renderer);
    shader_uniform_vec2(state.renderer.shaders[SHADER_SKY], "uv_offset", GLMS_VEC2_ZERO);
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

    // stars
    stars_render(self, center, state.renderer.textures[TEXTURE_STAR]);

    // sun
    plane_render(
        self, &state.renderer.textures[TEXTURE_SUN],
        (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
        celestial_model(self, SUN, center));

    // moon
    plane_render(
        self, &state.renderer.textures[TEXTURE_MOON],
        (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
        celestial_model(self, MOON, center));

    glDepthMask(GL_TRUE);

    // clouds
    const f32 clouds_size = 2048.0f, clouds_height = 192.0f;

    glDisable(GL_CULL_FACE);
    shader_uniform_vec2(
        state.renderer.shaders[SHADER_SKY], "uv_offset",
        (vec2s) {{
            fmodf(center.x, clouds_size) /  clouds_size,
            (fmodf(-center.z, clouds_size) / clouds_size) +
                ((f32) (self->world->ticks % (TOTAL_DAY_TICKS / 3))) / (TOTAL_DAY_TICKS / 3) }});
    shader_uniform_float(state.renderer.shaders[SHADER_SKY], "fog_near", self->fog_near * 5.0f);
    shader_uniform_float(state.renderer.shaders[SHADER_SKY], "fog_far", self->fog_far * 5.0f);
    plane_render(
        self, &state.renderer.textures[TEXTURE_CLOUDS],
        cloud_color,
        plane_model((vec3s) {{ center.x, clouds_height, center.z }},
            (vec3s) {{ radians(-90.0f), 0.0f, 0.0f }},
            (vec3s) {{ clouds_size, clouds_size, 0 }}));
    glEnable(GL_CULL_FACE);
}