#include "color.h"

// Conversion code adapted from https://gist.github.com/mattatz/44f081cac87e2f7c8980#file-labcolorspace-cginc-L29

vec3s rgb2xyz(vec3s c) {
    vec3s tmp;
    tmp.x = ( c.x > 0.04045 ) ? pow( ( c.x + 0.055 ) / 1.055, 2.4 ) : c.x / 12.92;
    tmp.y = ( c.y > 0.04045 ) ? pow( ( c.y + 0.055 ) / 1.055, 2.4 ) : c.y / 12.92,
    tmp.z = ( c.z > 0.04045 ) ? pow( ( c.z + 0.055 ) / 1.055, 2.4 ) : c.z / 12.92;
    const mat3s mat = (mat3s) { .col = {
		(vec3s) {{ 0.4124, 0.3576, 0.1805 }},
        (vec3s) {{ 0.2126, 0.7152, 0.0722 }},
        (vec3s) {{ 0.0193, 0.1192, 0.9505 }}
    }};
    return glms_vec3_scale(glms_mat3_mulv(mat, tmp), 100.0f);
}

vec3s xyz2lab(vec3s c) {
    vec3s n = glms_vec3_div(c, (vec3s){{95.047, 100, 108.883}});
    vec3s v;
    v.x = (n.x > 0.008856) ? pow(n.x, 1.0 / 3.0) : (7.787 * n.x) + (16.0 / 116.0);
    v.y = (n.y > 0.008856) ? pow(n.y, 1.0 / 3.0) : (7.787 * n.y) + (16.0 / 116.0);
    v.z = (n.z > 0.008856) ? pow(n.z, 1.0 / 3.0) : (7.787 * n.z) + (16.0 / 116.0);
    return (vec3s){{(116.0 * v.y) - 16.0, 500.0 * (v.x - v.y), 200.0 * (v.y - v.z)}};
}

vec3s rgb2lab(vec3s c) {
    vec3s lab = xyz2lab(rgb2xyz(c));
    return (vec3s){{lab.x / 100.0, 0.5 + 0.5 * (lab.y / 127.0), 0.5 + 0.5 * (lab.z / 127.0)}};
}

vec3s lab2xyz(vec3s c) {
    f32 fy = (c.x + 16.0) / 116.0;
    f32 fx = c.y / 500.0 + fy;
    f32 fz = fy - c.z / 200.0;
    return (vec3s){{95.047 * ((fx > 0.206897) ? fx * fx * fx : (fx - 16.0 / 116.0) / 7.787),
                    100.000 * ((fy > 0.206897) ? fy * fy * fy : (fy - 16.0 / 116.0) / 7.787),
                    108.883 * ((fz > 0.206897) ? fz * fz * fz : (fz - 16.0 / 116.0) / 7.787)}};
}

vec3s xyz2rgb(vec3s c) {
    const mat3s mat = (mat3s) { .col = {
            (vec3s) {{ 3.2406, -1.5372, -0.4986 }},
            (vec3s) {{ -0.9689, 1.8758, 0.0415 }},
            (vec3s) {{ 0.0557, -0.2040, 1.0570 }}
        }
    };
    vec3s v = glms_mat3_mulv(mat, glms_vec3_scale(c, 1.0f / 100.0f));
    vec3s r;
    r.x = (v.x > 0.0031308) ? ((1.055 * pow(v.x, (1.0 / 2.4))) - 0.055) : 12.92 * v.x;
    r.y = (v.y > 0.0031308) ? ((1.055 * pow(v.y, (1.0 / 2.4))) - 0.055) : 12.92 * v.y;
    r.z = (v.z > 0.0031308) ? ((1.055 * pow(v.z, (1.0 / 2.4))) - 0.055) : 12.92 * v.z;
    return r;
}

vec3s lab2rgb(vec3s c) {
    return xyz2rgb(lab2xyz((vec3s){{100.0 * c.x, 2.0 * 127.0 * (c.y - 0.5), 2.0 * 127.0 * (c.z - 0.5)}}));
}

vec3s rgb_brighten(vec3s rgb, f32 d) {
    vec3s lab = rgb2lab(rgb);
    return lab2rgb((vec3s){{lab.x + d, lab.y, lab.z}});
}

vec4s rgba_brighten(vec4s rgba, f32 d) {
    return glms_vec4(rgb_brighten((vec3s){{rgba.x, rgba.y, rgba.z}}, d), rgba.w);
}

vec4s rgba_lerp(vec4s rgba_a, vec4s rgba_b, f32 t) {
    vec3s lab_a = rgb2lab(glms_vec3(rgba_a));
    vec3s lab_b = rgb2lab(glms_vec3(rgba_b));

    return glms_vec4(
        lab2rgb(((vec3s){{(lerpf(lab_a.x, lab_b.x, t)),
                          (lerpf(lab_a.y, lab_b.y, t)),
                          (lerpf(lab_a.z, lab_b.z, t))}})),
        lerpf(rgba_a.w, rgba_b.w, t));
}

vec4s rgba_lerp3(vec4s rgba_a, vec4s rgba_b, vec4s rgba_c, f32 t) {
    if (t <= 0.5f) {
        return rgba_lerp(rgba_a, rgba_b, t * 2.0f);
    }

    return rgba_lerp(rgba_b, rgba_c, (t - 0.5f) * 2.0f);
}