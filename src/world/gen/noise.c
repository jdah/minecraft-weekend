#include "noise.h"
#include <noise1234.h>

f32 octave_compute(struct Octave *p, f32 seed, f32 x, f32 z) {
    f32 u = 1.0f, v = 0.0f;
    for (int i = 0; i < p->n; i++) {
        v += (1.0f / u) * noise3((x / 1.01f) * u, (z / 1.01f) * u, seed + (p->o * 32));
        u *= 2.0f;
    }
    return v;
}

struct Noise octave(s32 n, s32 o) {
    struct Noise result = {.compute = (FNoise) octave_compute};
    struct Octave params = {n, o};
    memcpy(&result.params, &params, sizeof(struct Octave));
    return result;
}

f32 combined_compute(struct Combined *p, f32 seed, f32 x, f32 z) {
    return p->n->compute(&p->n->params, seed, x + p->m->compute(&p->m->params, seed, x, z), z);
}

struct Noise combined(struct Noise *n, struct Noise *m) {
    struct Noise result = {.compute = (FNoise)combined_compute};
    struct Combined params = {n, m};
    memcpy(&result.params, &params, sizeof(struct Combined));
    return result;
}

f32 noise_compute(struct Basic *b, f32 seed, f32 x, f32 z) {
    return noise3(x, z, seed + (b->o * 32.0f));
}

struct Noise basic(s32 o) {
    struct Noise result = {.compute = (FNoise) noise_compute };
    struct Basic params = { .o = o };
    memcpy(&result.params, &params, sizeof(struct Basic));
    return result;
}

f32 expscale_compute(struct ExpScale *e, f32 seed, f32 x, f32 z) {
    f32 n = e->n->compute(&e->n->params, seed, x * e->scale, z * e->scale);
    return sign(n) * powf(fabsf(n), e->exp);
}

struct Noise expscale(struct Noise *n, f32 exp, f32 scale) {
    struct Noise result = {.compute = (FNoise) expscale_compute };
    struct ExpScale params = { .n = n, .exp = exp, .scale = scale };
    memcpy(&result.params, &params, sizeof(struct ExpScale));
    return result;
}