#ifndef STATE_H
#define STATE_H

#include "gfx/gfx.h"
#include "gfx/renderer.h"
#include "gfx/window.h"
#include "util/util.h"
#include "world/sky.h"
#include "world/world.h"
#include "ui/ui.h"

struct State {
    struct Window *window;
    struct Renderer renderer;
    struct World world;
    struct UI ui;
    size_t ticks;
};

// global state
extern struct State state;

#endif