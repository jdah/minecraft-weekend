#version 330

uniform vec4 color;

out vec4 frag_color;

void main() {
    frag_color = color;
}