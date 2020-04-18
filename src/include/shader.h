#ifndef SHADER_H
#define SHADER_H

#include "gfx.h"
#include "util.h"
#include "camera.h"
#include "texture.h"

struct VertexAttr {
    GLuint index;
    const GLchar *name;
};

struct Shader {
    GLuint handle, vs_handle, fs_handle;
};


struct Shader shader_create(char *vs_path, char *fs_path, size_t n, struct VertexAttr attributes[]);
void shader_destroy(struct Shader self);
void shader_bind(struct Shader self);
void shader_uniform_mat4(struct Shader self, char *name, mat4s m);
void shader_uniform_camera(struct Shader self, struct Camera camera);
void shader_uniform_texture2D(struct Shader self, char *name, struct Texture texture, GLuint n);

#endif