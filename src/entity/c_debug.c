#include "c_debug.h"
#include "ecs.h"

static void render(struct DebugComponent *c_debug, struct Entity entity) {

}

void c_debug_init(struct ECS *ecs) {
    ecs_register(C_DEBUG, struct DebugComponent, ecs, ((union ECSSystem) {
        .init = NULL,
        .destroy = NULL,
        .render = (ECSSubscriber) render,
        .update = NULL,
        .tick = NULL
    }));
}