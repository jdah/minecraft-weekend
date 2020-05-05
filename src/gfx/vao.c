#include "vao.h"

struct VAO vao_create() {
    struct VAO self;
    glGenVertexArrays(1, &self.handle);
    return self;
}

void vao_destroy(struct VAO self) {
    glDeleteVertexArrays(1, &self.handle);
}

void vao_bind(struct VAO self) {
    glBindVertexArray(self.handle);
}

void vao_attr(
    struct VAO self, struct VBO vbo, GLuint index, GLint size, GLenum type,
    GLsizei stride, size_t offset) {
    vao_bind(self);
    vbo_bind(vbo);

    // NOTE: glVertexAttribPointer will AUTO-CONVERT integer values to floating point.
    // Integer vertex attributes must be specified with glVertexAttribIPointer.
    // THIS IS EVIL. OpenGL is bad. Who designed this to fail silently?
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            glVertexAttribIPointer(index, size, type, stride, (void *) offset);
            break;
        default:
            glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *) offset);
            break;
    }
    glEnableVertexAttribArray(index);
}