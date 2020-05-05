#ifndef COLOR_H
#define COLOR_H

#include "../util/util.h"

// converts RGBA in hexadecimal u32 in [0, 255] to vec4s in [0, 1]
#define RGBAX2F(c) ((vec4s) {{\
        ((c & 0xFF000000) >> 24) / 255.0f,\
        ((c & 0x00FF0000) >> 16) / 255.0f,\
        ((c & 0x0000FF00) >>  8) / 255.0f,\
        ((c & 0x000000FF) >>  0) / 255.0f\
    }})

vec3s rgb2lab(vec3s rgb);
vec3s hsv2lab(vec3s hsv);

vec3s rgb_brighten(vec3s rgb, f32 d);
vec4s rgba_brighten(vec4s rgba, f32 d);

vec4s rgba_lerp(vec4s rgba_a, vec4s rgba_b, f32 t);
vec4s rgba_lerp3(vec4s rgba_a, vec4s rgba_b, vec4s rgba_c, f32 t);

#endif