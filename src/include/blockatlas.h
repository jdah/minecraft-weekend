#ifndef BLOCKATLAS_H
#define BLOCKATLAS_H

#include "util.h"
#include "gfx.h"
#include "texture.h"

#define BLOCK_ATLAS_FPS 5
#define BLOCK_ATLAS_FRAMES 5

struct BlockAtlas {
    struct Texture frames[BLOCK_ATLAS_FRAMES];
    struct Atlas atlas;
    size_t ticks;
};

struct BlockAtlas blockatlas_create(char *path);
void blockatlas_destroy(struct BlockAtlas *self);
void blockatlas_tick(struct BlockAtlas *self);

#endif