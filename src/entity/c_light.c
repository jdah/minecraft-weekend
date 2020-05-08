#include "c_light.h"
#include "ecs.h"
#include "../world/world.h"
#include "../world/light.h"

static void tick(struct LightComponent *c_light, struct Entity entity) {
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);

    bool changed = ivec3scmp(c_position->block, c_light->last.pos) ||
        c_light->flags.enabled != c_light->last.enabled;

    if (changed) {
        torchlight_remove(entity.ecs->world, c_light->last.pos);

        // reset block light if it was removed
        struct Block block = BLOCKS[world_get_block(entity.ecs->world, c_light->last.pos)];
        if (block.can_emit_light) {
            torchlight_add(
                entity.ecs->world,
                c_light->last.pos,
                block.get_torchlight(entity.ecs->world, c_light->last.pos));
        }
    }

    if (c_light->flags.enabled && changed) {
        torchlight_add(entity.ecs->world, c_position->block, c_light->light);

        c_light->last.pos = c_position->block;
        c_light->last.light = c_light->light;
    }

    c_light->last.enabled = c_light->flags.enabled;
}

void c_light_init(struct ECS *ecs) {
    ecs_register(C_LIGHT, struct LightComponent, ecs, ((union ECSSystem) {
        .init = NULL,
        .destroy = NULL,
        .render = NULL,
        .update = NULL,
        .tick = (ECSSubscriber) tick
    }));
}