#ifndef VAO_H
#define VAO_H

#include "../util/util.h"
#include "gfx.h"
#include "vbo.h"

struct VAO {
    GLuint handle;
};

struct VAO vao_create();
void vao_destroy(struct VAO self);
void vao_bind(struct VAO self);
void vao_attr(
    struct VAO self, struct VBO vbo, GLuint index, GLint size, GLenum type,
    GLsizei stride, size_t offset);

#endif