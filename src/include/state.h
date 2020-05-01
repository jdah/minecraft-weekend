#ifndef STATE_H
#define STATE_H

#include "gfx.h"
#include "util.h"
#include "world.h"
#include "window.h"
#include "renderer.h"

struct State {
    struct Window *window;
    struct Renderer renderer;
    struct World world;
    size_t ticks;
};

// global state
extern struct State state;

#endif