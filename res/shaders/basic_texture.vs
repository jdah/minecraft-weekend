#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform mat4 m, v, p;

out vec2 v_uv;

void main() {
    gl_Position = p * v * m * vec4(position, 1.0);
    v_uv = uv;
}