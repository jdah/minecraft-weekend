#include "ui.h"

#define DECL_UI_FN(_name)\
    void ui_##_name(struct UI *self) {\
        for (size_t i = 0; i < self->components.count; i++) {\
            struct UIComponent c = self->components.elements[i];\
            if (c._name != NULL && c.enabled) {\
                c._name(c.component);\
            }\
        }\
    }

DECL_UI_FN(destroy)
DECL_UI_FN(render)
DECL_UI_FN(update)
DECL_UI_FN(tick)