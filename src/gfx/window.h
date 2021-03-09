#ifndef WINDOW_H
#define WINDOW_H

#include "gfx.h"
#include "../util/util.h"

struct Button {
    bool down, last, last_tick, pressed, pressed_tick;
};


struct Mouse {
    struct Button buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s position, delta;
};

struct Keyboard {
    struct Button keys[GLFW_KEY_LAST];
};

typedef void (*FWindow)();

struct Window {
    GLFWwindow *handle;
    ivec2s size;
    FWindow init, destroy, tick, update, render;
    struct Mouse mouse;
    struct Keyboard keyboard;

    // timing variables
    u64 last_second;
    u64 frames, fps, last_frame, frame_delta;
    u64 ticks, tps, tick_remainder;
};

// global window
extern struct Window window;

void window_loop();
void window_create(FWindow init, FWindow destroy, FWindow tick,  FWindow update, FWindow render);
void mouse_set_grabbed(bool grabbed);
bool mouse_get_grabbed();

#endif