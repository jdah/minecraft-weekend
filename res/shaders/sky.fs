#version 330

uniform sampler2D tex;
uniform bool use_tex;
uniform vec2 uv_offset;

uniform vec4 color;
uniform vec4 fog_color;
uniform float fog_near;
uniform float fog_far;

in vec2 v_uv;
in vec3 v_viewpos;

out vec4 frag_color;

void main() {
    frag_color = color * (use_tex ? texture(tex, v_uv + uv_offset) : vec4(1.0));

    // TODO: this is bad for performance
    if (frag_color.a < 0.001) {
        discard;
    }
    
    float fog = smoothstep(fog_near, fog_far, length(v_viewpos));
    frag_color = vec4(mix(frag_color, fog_color, fog).rgb, frag_color.a);
}