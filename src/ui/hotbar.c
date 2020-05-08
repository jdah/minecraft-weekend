#include "hotbar.h"
#include "ui.h"

#include "../state.h"
#include "../gfx/renderer.h"
#include "../block/block.h"

const f32 SLOT_PIXELS = 40.0f;

const f32 ICON_OFFSET_PIXELS = 6.0f;
const f32 ICON_SIZE_PIXELS = 28.0f;

static void render_icon(vec2s offset, enum BlockId block_id) {
    mat4s m = glms_mat4_identity();
    m = glms_translate(m, (vec3s) {{
        offset.x + ICON_OFFSET_PIXELS,
        offset.y + ICON_OFFSET_PIXELS,
        0.0f }});
    m = glms_scale(m, (vec3s) {{ ICON_SIZE_PIXELS, ICON_SIZE_PIXELS, 1.0f }});

    vec2s uv_min, uv_max;
    atlas_get(
        state.renderer.block_atlas.atlas,
        BLOCKS[block_id].get_texture_location(&state.world, GLMS_IVEC3_ZERO, UP),
        &uv_min, &uv_max);
    renderer_quad_texture(
        &state.renderer, state.renderer.block_atlas.atlas.texture,
        (vec2s) {{ 1.0f, 1.0f }},
        (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
        uv_min, uv_max, m);
}

static void render(struct UIHotbar *self) {
    vec2s base_offset = (vec2s) {{ (state.window->size.x - (HOTBAR_SLOTS * SLOT_PIXELS)) / 2.0f, 16.0f }};
    
    for (size_t i = 0; i < HOTBAR_SLOTS; i++) {
        vec2s offset = glms_vec2_add((vec2s) {{ i * SLOT_PIXELS, 0 }}, base_offset);

        renderer_quad_texture(
            &state.renderer, state.renderer.textures[TEXTURE_HOTBAR],
            (vec2s) {{ SLOT_PIXELS, SLOT_PIXELS }},
            (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
            (vec2s) {{ 0.0f, 0.0f }}, (vec2s) {{ 0.5f, 1.0f }},
            glms_translate_make((vec3s) {{ offset.x, offset.y, 0.0f }}));

        if (i == self->index) {
            renderer_quad_texture(
                &state.renderer, state.renderer.textures[TEXTURE_HOTBAR],
                (vec2s) {{ SLOT_PIXELS, SLOT_PIXELS }},
                (vec4s) {{ 1.0f, 1.0f, 1.0f, 1.0f }},
                (vec2s) {{ 0.5f, 0.0f }}, (vec2s) {{ 1.0f, 1.0f }},
                glms_translate_make((vec3s) {{ offset.x, offset.y, 0.0f }}));
        }
        
        render_icon(offset, self->values[i]);
    }
}

static void update(struct UIHotbar *self) {
    for (size_t i = 0; i < 10; i++) {
        if (state.window->keyboard.keys[GLFW_KEY_0 + i].pressed) {
            self->index = i == 0 ? 9 : (i - 1);
        }
    }
}

struct UIComponent hotbar_init(struct UIHotbar *self) {
    self->index = 0;

    memcpy(self->values, (enum BlockId[]) {
            GRASS,
            DIRT,
            STONE,
            COBBLESTONE,
            PLANKS,
            LOG,
            GLASS,
            ROSE,
            TORCH,
            SAND,
            LEAVES
        }, HOTBAR_SLOTS * sizeof(enum BlockId));

    return (struct UIComponent) {
        .component = self,
        .render = (FUIComponent) render,
        .update = (FUIComponent) update,
        .tick = NULL
    };
}