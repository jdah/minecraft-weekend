#include "ecs.h"
#include "position.h"

static void tick(struct PositionComponent *position_component, struct Entity entity) {
    glms_vec3_print(position_component->position, stdout);
}

void position_init(struct ECS *ecs) {
    ecs_register(C_POSITION, struct PositionComponent, ecs, ((union ECSSystem) {
        .init = NULL,
        .destroy = NULL,
        .render = NULL,
        .update = NULL,
        .tick = (ECSSubscriber) tick
    }));
}