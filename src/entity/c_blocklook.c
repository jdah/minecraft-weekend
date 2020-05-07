#include "c_blocklook.h"
#include "ecs.h"

#include "../world/world.h"
#include "../state.h"
#include "../gfx/renderer.h"

static bool block_raycast(struct World *world, ivec3s pos) {
    struct Block block = BLOCKS[world_get_block(world, pos)];
    return block.id != AIR && !block.liquid;
}

static void tick(struct BlockLookComponent *c_blocklook, struct Entity entity) {
    struct CameraComponent *c_camera = ecs_get(entity, C_CAMERA);

    ivec3s pos;
    enum Direction dir;

    if (ray_block((struct Ray) {
        .origin = c_camera->camera.position,
        .direction = c_camera->camera.direction
        }, c_blocklook->radius, entity.ecs->world, (bool (*)(void *, ivec3s)) block_raycast, &pos, &dir)) {
        c_blocklook->hit = true;
        c_blocklook->pos = pos;
        c_blocklook->face = dir;
    } else {
        c_blocklook->hit = false;
    }
}

static void render(struct BlockLookComponent *c_blocklook, struct Entity entity) {
    if (c_blocklook->flags.render && c_blocklook->hit) {
        AABB aabb;
        BLOCKS[world_get_block(entity.ecs->world, c_blocklook->pos)]
            .get_aabb(entity.ecs->world, c_blocklook->pos, aabb);
        glms_aabb_scale(aabb, (vec3s) {{ 1.005f, 1.005f, 1.005f }}, aabb);
        renderer_aabb(
            &state.renderer, aabb,
            (vec4s) {{ 1.0f, 1.0f, 1.0f,
                ((state.ticks % 40) > 20 ?
                    ((state.ticks % 40) / 40.0f) :
                    (1.0f - ((state.ticks % 40) / 40.0f))) * 0.3f }},
            glms_mat4_identity(),
            FILL_MODE_FILL);
    }
}

void c_blocklook_init(struct ECS *ecs) {
    ecs_register(C_BLOCKLOOK, struct BlockLookComponent, ecs, ((union ECSSystem) {
        .init = NULL,
        .destroy = NULL,
        .render = (ECSSubscriber) render,
        .update = NULL,
        .tick = (ECSSubscriber) tick
    }));
}