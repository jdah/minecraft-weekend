#ifndef BITMAP_H
#define BITMAP_H

#include <stdlib.h>
#include "types.h"

typedef u8 *Bitmap;

#define BITMAP_SIZE_TO_BYTES(_sz) ((((_sz) / 64) + 1) * sizeof(u64))

static inline Bitmap bitmap_alloc(u64 sz) { return (Bitmap) malloc(BITMAP_SIZE_TO_BYTES(sz)); }
static inline Bitmap bitmap_calloc(u64 sz) { return (Bitmap) calloc(1, BITMAP_SIZE_TO_BYTES(sz)); }
static inline Bitmap bitmap_realloc(Bitmap self, u64 sz) { return (Bitmap) realloc((void *) self, BITMAP_SIZE_TO_BYTES(sz)); }
static inline void bitmap_set(Bitmap self, u64 n) { ((u8 *) self)[n / 8] |= (1 << (n % 8)); }
static inline bool bitmap_get(Bitmap self, u64 n) { return (((u8 *) self)[n / 8] & (1 << (n % 8))) != 0; }
static inline void bitmap_clr(Bitmap self, u64 n) { ((u8 *) self)[n / 8] &= ~(1 << (n % 8)); }

#endif