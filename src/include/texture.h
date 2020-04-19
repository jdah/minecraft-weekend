#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfx.h"
#include "util.h"

struct Texture {
    GLuint handle;
    ivec2s size;
    vec2s uv_unit;
};

void texture_load_pixels(char *path, u8 **pixels_out, size_t *width_out, size_t *height_out);
struct Texture texture_create_from_pixels(u8 *pixels, size_t width, size_t height);
struct Texture texture_create_from_path(char *path);
void texture_destroy(struct Texture self);
void texture_bind(struct Texture self);

struct Atlas {
    struct Texture texture;
    ivec2s size, sprite_size;
    vec2s sprite_unit;
};

struct Atlas atlas_create_from_texture(struct Texture texture, ivec2s sprite_size);
struct Atlas atlas_create(char *path, ivec2s sprite_size);
void atlas_destroy(struct Atlas self);
vec2s atlas_offset(struct Atlas self, ivec2s pos);

#endif