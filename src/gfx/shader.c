#include "shader.h"

static void _log_and_fail(
    GLint handle, const char *adverb, const char *path,
    void (*getlog)(GLuint, GLsizei, GLsizei *, GLchar*),
    void (*getiv)(GLuint, GLenum, GLint *)) {
    GLint loglen;
    getiv(handle, GL_INFO_LOG_LENGTH, &loglen);

    char *logtext = calloc(1, loglen);
    getlog(handle, loglen, NULL, logtext);
    fprintf(stderr, "Error %s shader at %s:\n%s", adverb, path, logtext);

    free(logtext);
    exit(1);
}

static GLint _compile(char *path, GLenum type) {
    FILE *f;
    char *text;
    long len;

    f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "error loading shader at %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);
    text = calloc(1, len);
    assert(text != NULL);
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);

    GLuint handle = glCreateShader(type);
    glShaderSource(handle,1, (const GLchar *const *) &text, (const GLint *) &len);
    glCompileShader(handle);

    GLint compiled;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    // Check OpenGL logs if compilation failed
    if (compiled == 0) {
        _log_and_fail(handle, "compiling", path, glGetShaderInfoLog, glGetShaderiv);
    }

    free(text);
    return handle;
}

struct Shader shader_create(char *vs_path, char *fs_path, size_t n, struct VertexAttr attributes[]) {
    struct Shader self;
    self.vs_handle = _compile(vs_path, GL_VERTEX_SHADER);
    self.fs_handle = _compile(fs_path, GL_FRAGMENT_SHADER);
    self.handle = glCreateProgram();

    // Link shader program
    glAttachShader(self.handle, self.vs_handle);
    glAttachShader(self.handle, self.fs_handle);

    // Bind vertex attributes
    for (size_t i = 0; i < n; i++) {
        glBindAttribLocation(self.handle, attributes[i].index, attributes[i].name);
    }

    glLinkProgram(self.handle);

    // Check link status
    GLint linked;
    glGetProgramiv(self.handle, GL_LINK_STATUS, &linked);

    if (linked == 0) {
        char buf[512];
        snprintf(buf, 512, "[%s, %s]", vs_path, fs_path);
        _log_and_fail(self.handle, "linking", buf, glGetProgramInfoLog, glGetProgramiv);
    }

    return self;
}

void shader_destroy(struct Shader self) {
    glDeleteProgram(self.handle);
    glDeleteShader(self.vs_handle);
    glDeleteShader(self.fs_handle);
}

void shader_bind(struct Shader self) {
    glUseProgram(self.handle);
}

void shader_uniform_mat4(struct Shader self, char *name, mat4s m) {
    glUniformMatrix4fv(glGetUniformLocation(self.handle, name), 1, GL_FALSE, (const GLfloat *) &m.raw);
}

void shader_uniform_view_proj(struct Shader self, struct ViewProj view_proj) {
    shader_uniform_mat4(self, "p", view_proj.proj);
    shader_uniform_mat4(self, "v", view_proj.view);
}

void shader_uniform_texture2D(struct Shader self, char *name, struct Texture texture, GLuint n) {
    glActiveTexture(GL_TEXTURE0 + n);
    texture_bind(texture);
    glUniform1i(glGetUniformLocation(self.handle, (const GLchar *) name), n);
}

void shader_uniform_float(struct Shader self, char *name, f32 f) {
    glUniform1f(glGetUniformLocation(self.handle, name), f);
}

void shader_uniform_vec2(struct Shader self, char *name, vec2s v) {
    glUniform2f(glGetUniformLocation(self.handle, name), v.x, v.y);
}

void shader_uniform_vec3(struct Shader self, char *name, vec3s v) {
    glUniform3f(glGetUniformLocation(self.handle, name), v.x, v.y, v.z);
}

void shader_uniform_vec4(struct Shader self, char *name, vec4s v) {
    glUniform4f(glGetUniformLocation(self.handle, name), v.x, v.y, v.z, v.w);
}

void shader_uniform_int(struct Shader self, char *name, int v) {
    glUniform1i(glGetUniformLocation(self.handle, name), v);
}
void shader_uniform_uint(struct Shader self, char *name, unsigned int v) {
    glUniform1ui(glGetUniformLocation(self.handle, name), v);
}