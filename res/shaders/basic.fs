#version 410

uniform sampler2D tex;

in vec4 v_color;
in vec2 v_uv;
in vec3 v_viewpos;

out vec4 frag_color;

const vec4 fog_color = vec4(0.5, 0.8, 0.9, 1.0);
const float fog_near = (12 * 16) - 24;
const float fog_far = (12 * 16) - 12;

void main() {
    frag_color = texture(tex, v_uv) * v_color;

    float fog = smoothstep(fog_near, fog_far, length(v_viewpos));
    frag_color = mix(frag_color, fog_color, fog);
}