#ifndef AABB_H
#define AABB_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#pragma GCC diagnostic pop

#include "types.h"
#include "fmath.h"

typedef vec3s AABB[2];

#define AABB_COPY(_aabb) { (_aabb)[0], (_aabb)[1] }

// returns the depth of collision of aabb a into aabb b
static inline vec3s glms_aabb_aabb_depth(vec3s a[2], vec3s b[2]) {
    vec3s result, a_c = glms_aabb_center(a), b_c = glms_aabb_center(b);

    for (size_t i = 0; i < 3; i++) {
        if (a_c.raw[i] < b_c.raw[i]) {
            result.raw[i] = a[1].raw[i] - b[0].raw[i];
        } else {
            result.raw[i] = b[1].raw[i] - a[0].raw[i];
        }
    }

    return result;
}

// scales an AABB the specified amounts along each axis
static inline void glms_aabb_scale(AABB box, vec3s scale, AABB dest) {
    vec3s center = glms_aabb_center(box);
    vec3s size = glms_vec3_sub(box[1], box[0]);

    vec3s new_size = glms_vec3_mul(size, scale);
    vec3s half_new_size = glms_vec3_scale(new_size, 0.5f);

    // scaled AABB centered around same center
    dest[0] = glms_vec3_add(glms_vec3_scale(half_new_size, -1.0f), center);
    dest[1] = glms_vec3_add(glms_vec3_sub(new_size, half_new_size), center);
}

#endif