#ifndef STATE_H
#define STATE_H

#include "gfx.h"
#include "util.h"
#include "shader.h"
#include "world.h"
#include "window.h"
#include "blockatlas.h"

struct State {
    struct Window *window;
    struct Shader shader;
    struct BlockAtlas block_atlas;
    struct World world;

    bool wireframe;
};

// global state
extern struct State state;

#endif