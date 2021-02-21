#version 330

uniform sampler2D tex;
uniform bool use_tex;

in vec4 v_color;
in vec2 v_uv;

out vec4 frag_color;

void main() {
    frag_color = (use_tex ? texture(tex, v_uv) : vec4(1.0)) * v_color;
}