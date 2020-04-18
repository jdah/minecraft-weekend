#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform mat4 m, v, p;

out vec4 v_color;
out vec2 v_uv;

void main() {
    gl_Position = p * v * m * vec4(position, 1.0);
    v_color = vec4(p[0][3], 1.0, 0.0, 1.0);
    v_uv = uv;
}