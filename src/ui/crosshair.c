#include "crosshair.h"
#include "ui.h"

#include "../state.h"
#include "../gfx/renderer.h"

static void render(struct UICrosshair *self) {
}

struct UIComponent crosshair_init(struct UICrosshair *self) {
    self->enabled = true;

    return (struct UIComponent) {
        .component = self,
        .render = (FUIComponent) render
    };
}
