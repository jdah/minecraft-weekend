#include "include/blockatlas.h"
#include "include/block.h"

static void copy_pixels(u8 *pixels, ivec2s image_size, ivec2s size, ivec2s from, ivec2s to) {
    for (s32 j = 0; j < size.y; j++) {
        for (s32 i = 0; i < size.x; i++) {
            pixels[((to.y + j) * image_size.x + (to.x + i)) * 4] =
                pixels[((from.y + j) * image_size.x + (from.x + i)) * 4];
        }
    }
}

#define ATLAS_SIZE ((ivec2s) {{ 256, 256 }})
#define ATLAS_SIZE_SPRITES ((ivec2s) {{ 16, 16 }})
#define ATLAS_SPRITE_SIZE ((ivec2s) {{ 16, 16 }})

static void copy_offset(u8 *pixels, ivec2s from, ivec2s to) {
    copy_pixels(
        pixels, ATLAS_SIZE, ATLAS_SPRITE_SIZE,
        glms_ivec2_mul(ATLAS_SPRITE_SIZE, (ivec2s) {{ from.x, ATLAS_SIZE_SPRITES.y - from.y - 1 }}),
        glms_ivec2_mul(ATLAS_SPRITE_SIZE, (ivec2s) {{ to.x, ATLAS_SIZE_SPRITES.y - to.y - 1 }}));
}

struct BlockAtlas blockatlas_create(char *path) {
    struct BlockAtlas self;
    self.ticks = 0;

    u8 *pixels;
    size_t width, height;
    texture_load_pixels(path,  &pixels, &width, &height);
    const size_t pixels_size = width * height * 4;

    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        u8 *new_pixels = malloc(pixels_size);
        memcpy(new_pixels, pixels, pixels_size);

        for (size_t n = 0; n < BLOCK_ID_LAST; n++) {
            struct Block block = BLOCKS[n];
            if (block.id == 0 || !block.is_animated()) {
                continue;
            }

            ivec2s frames[BLOCK_ATLAS_FRAMES];
            block.get_animation_frames(frames);
            copy_offset(new_pixels, frames[i], frames[0]);
        }

        self.frames[i] = texture_create_from_pixels(new_pixels, width, height);
        free(new_pixels);
    }

    self.atlas = atlas_create_from_texture(self.frames[0], ATLAS_SPRITE_SIZE);

    free(pixels);
    return self;
}

void blockatlas_destroy(struct BlockAtlas *self) {
    for (size_t i = 0; i < BLOCK_ATLAS_FRAMES; i++) {
        texture_destroy(self->frames[i]);
    }
}

void blockatlas_tick(struct BlockAtlas *self) {
    size_t frame = (self->ticks % 60) / (60 / BLOCK_ATLAS_FRAMES);
    self->atlas.texture = self->frames[frame];
    self->ticks++;
}