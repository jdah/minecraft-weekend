#ifndef WINDOW_H
#define WINDOW_H

#include "gfx.h"
#include "../util/util.h"

enum ButtonState {
    BUTTON_STATE_DOWN         = 0x1,
    BUTTON_STATE_LAST         = 0x2,
    BUTTON_STATE_LAST_TICK    = 0x4,
    BUTTON_STATE_PRESSED      = 0x8,
    BUTTON_STATE_PRESSED_TICK = 0x10,
};

struct Mouse {
    enum ButtonState buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s position, delta;
};

struct Keyboard {
    enum ButtonState keys[GLFW_KEY_LAST];
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

#endif