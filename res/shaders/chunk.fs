#version 330

uniform sampler2D tex;

in vec4 v_color;
in vec2 v_uv;
in vec3 v_viewpos;

out vec4 frag_color;

uniform vec4 fog_color;
uniform float fog_near;
uniform float fog_far;

void main() {
    frag_color = texture(tex, v_uv) * v_color;
    
    // TODO: I think this is awful for performance
    if (frag_color.a == 0.0) {
        discard;
    }

    frag_color *= 1.0 - smoothstep(fog_near, fog_far, length(v_viewpos));
}