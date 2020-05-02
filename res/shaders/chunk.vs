#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in uint color;

uniform mat4 m, v, p;

out vec4 v_color;
out vec2 v_uv;
out vec3 v_viewpos;

void main() {
    gl_Position = p * v * m * vec4(position, 1.0);
    
    // TODO: sunlight
    // 'color' is packed:
    // - (4) sunlight intensity
    // - (4) R
    // - (4) G
    // - (4) B
    // - (4) intensity
    v_color = vec4(
        vec3(
            ((color & 0x0F000U) >> 12U) / 16.0,
            ((color & 0x00F00U) >>  8U) / 16.0,
            ((color & 0x000F0U) >>  4U) / 16.0
        ) * ((color & 0x0000FU) / 16.0),
        1.0
    );

    v_uv = uv;
    v_viewpos = ((v * m) * vec4(position, 1.0)).xyz;
}