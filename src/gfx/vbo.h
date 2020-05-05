#ifndef VBO_H
#define VBO_H

#include "../util/util.h"
#include "gfx.h"

struct VBO {
    GLuint handle;
    GLint type;
    bool dynamic;
};

struct VBO vbo_create(GLint type, bool dynamic);

void vbo_destroy(struct VBO self);

void vbo_bind(struct VBO self);

void vbo_buffer(struct VBO self, void *data, size_t offset, size_t count);

#endif