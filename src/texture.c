#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "include/texture.h"

struct Texture texture_create(char *path) {
    struct Texture self;
    int width, height, channels;

    // OpenGL has texture coordinates with (0, 0) on bottom
    stbi_set_flip_vertically_on_load(true);
    unsigned char *image = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    assert(image != NULL);
    self.size = (ivec2s) {{width, height}};

    glGenTextures(1, &self.handle);
    glBindTexture(GL_TEXTURE_2D, self.handle);

    // No interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return self;
}

void texture_destroy(struct Texture self) {
    glDeleteTextures(1, &self.handle);
}

void texture_bind(struct Texture self) {
    glBindTexture(GL_TEXTURE_2D, self.handle);
}

struct Atlas atlas_create(char *path, ivec2s sprite_size) {
    struct Atlas self;
    self.texture = texture_create(path);
    self.sprite_size = sprite_size;
    self.sprite_unit = glms_vec2_div(IVEC2S2V(self.sprite_size), IVEC2S2V(self.texture.size));
    self.size = glms_ivec2_div(self.texture.size, self.sprite_size);
    return self;
}

void atlas_destroy(struct Atlas self) {
    texture_destroy(self.texture);
}

vec2s atlas_offset(struct Atlas self, ivec2s pos) {
    // Invert Y coordinates so sprite (0, 0) is top-left
    return glms_vec2_mul(IVEC2S2V(((ivec2s) {{ pos.x, self.sprite_size.y - pos.y - 1}})), self.sprite_unit);
}