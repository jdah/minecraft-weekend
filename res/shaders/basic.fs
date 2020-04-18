#version 410

uniform sampler2D tex;

in vec4 v_color;
in vec2 v_uv;

out vec4 frag_color;

void main() {
    frag_color = texture(tex, v_uv);
    // frag_color = vec4(1.0);
}