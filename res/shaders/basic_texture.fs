#version 330

uniform sampler2D tex;
uniform vec4 color;

in vec2 v_uv;

out vec4 frag_color;

void main() {
    frag_color = color * texture(tex, v_uv);
}