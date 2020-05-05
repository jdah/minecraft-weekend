#include "blockatlas.h"
#include "../block/block.h"
#include "../state.h"

static void copy_pixels(u8 *pixels, ivec2s image_size, ivec2s size, ivec2s from, ivec2s to) {
    for (s32 j = 0; j < size.y; j++) {
        for (s32 i = 0; i < size.x; i++) {
            pixels[((to.y + j) * image_size.x + (to.x + i)) * 4] =
                pixels[((from.y + j) * image_size.x + (from.x + i)) * 4];
        }
    }
}

static void copy_offset(struct BlockAtlas *atlas, u8 *pixels, ivec2s from, ivec2s to) {
    copy_pixels(
        pixels, atlas->size, atlas->sprite_size,
        glms_ivec2_mul(atlas->sprite_size, (ivec2s) {{ from.x, atlas->size_sprites.y - from.y - 1 }}),
        glms_ivec2_mul(atlas->sprite_size, (ivec2s) {{ to.x, atlas->size_sprites.y - to.y - 1 }}));
}

struct BlockAtlas blockatlas_create(char *path, ivec2s sprite_size) {
    struct BlockAtlas self;
    self.ticks = 0;
    self.sprite_size = sprite_size;

    u8 *pixels;
    size_t width, height;
    texture_load_pixels(path,  &pixels, &width, &height);
    const size_t pixels_size = width * height * 4;

    self.size = (ivec2s) {{ width, height }};
    self.size_sprites = glms_ivec2_div(self.size, self.sprite_size);

    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        u8 *new_pixels = malloc(pixels_size);
        memcpy(new_pixels, pixels, pixels_size);

        for (size_t n = 0; n <= BLOCK_ID_LAST; n++) {
            struct Block block = BLOCKS[n];
            if (block.id == 0 || !block.animated) {
                continue;
            }

            ivec2s frames[BLOCK_ATLAS_FRAMES];
            block.get_animation_frames(frames);
            copy_offset(&self, new_pixels, frames[i], frames[0]);
        }

        self.frames[i] = texture_create_from_pixels(new_pixels, width, height);
        free(new_pixels);
    }

    self.atlas = atlas_create_from_texture(self.frames[0], self.sprite_size);

    free(pixels);
    return self;
}

void blockatlas_destroy(struct BlockAtlas *self) {
    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        texture_destroy(self->frames[i]);
    }
}

void blockatlas_update(struct BlockAtlas *self) {
    size_t frame = (state.ticks / (TICKRATE / BLOCK_ATLAS_FPS)) % BLOCK_ATLAS_FRAMES;
    self->atlas.texture = self->frames[frame];
}