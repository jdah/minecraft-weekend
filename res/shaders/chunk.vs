#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 color;

uniform mat4 m, v, p;

out vec4 v_color;
out vec2 v_uv;
out vec3 v_viewpos;

void main() {
    gl_Position = p * v * m * vec4(position, 1.0);
    v_color = vec4(color, 1.0);
    v_uv = uv;
    v_viewpos = ((v * m) * vec4(position, 1.0)).xyz;
}