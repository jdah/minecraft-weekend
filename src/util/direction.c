#include "direction.h"
#include <string.h>
#include <assert.h>

const ivec3s DIRECTION_IVEC[6] = {
    {{  0,  0, -1 }},
    {{  0,  0,  1 }},
    {{  1,  0,  0 }},
    {{ -1,  0,  0 }},
    {{  0,  1,  0 }},
    {{  0, -1,  0 }},
};

const vec3s DIRECTION_VEC[6] = {
    {{  0,  0, -1 }},
    {{  0,  0,  1 }},
    {{  1,  0,  0 }},
    {{ -1,  0,  0 }},
    {{  0,  1,  0 }},
    {{  0, -1,  0 }},
};

enum Direction _ivec3s2dir(ivec3s v) {
    for (size_t i = 0; i < 6; i++) {
        if (!memcmp(&DIR2IVEC3S(i), &v, sizeof(ivec3s))) {
            return i;
        }
    }

    assert(false);
    return -1;
}