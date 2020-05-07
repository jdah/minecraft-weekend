#ifndef UTIL_H
#define UTIL_H

#define TICKRATE 60

#define PACKED __attribute__((__packed__))

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#pragma GCC diagnostic pop

#include "ivec2.h"
#include "ivec2s.h"
#include "ivec3.h"
#include "ivec3s.h"
#include "aabb.h"

#include "fmath.h"
#include "time.h"
#include "direction.h"
#include "types.h"
#include "bitmap.h"
#include "color.h"

// TODO: move elsewhere
static inline void memsetl(void *dst, u64 v, size_t sz) {
    assert(sz % 8 == 0);
    size_t n = sz / 8;
    for (size_t i = 0; i < n; i++) {
        ((u64*)(dst))[i] = v;
    }
}

#endif