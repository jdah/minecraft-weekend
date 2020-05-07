#ifndef ECS_H
#define ECS_H

#include "../util/util.h"

// forward declarations
struct World;

#define ecs_register(_id, _C, _ecs, _system) _ecs_register_internal((_id), sizeof(_C), (_ecs), (_system))

typedef u64 ECSTag;

#include "ecscomponents.h"
#include "ecstypes.h"

#define ECSEVENT_LAST ECS_TICK
enum ECSEvent {
    ECS_INIT = 0, ECS_DESTROY, ECS_RENDER, ECS_UPDATE, ECS_TICK
};


typedef void (*ECSSubscriber)(void *, struct Entity);

union ECSSystem {
    struct {
        ECSSubscriber init, destroy, render, update, tick;
    };

    ECSSubscriber subscribers[ECSEVENT_LAST + 1];
};

struct ComponentList {
    union ECSSystem system;
    void *components;
    size_t component_size;
};

enum ECSTagValues {
    ECS_TAG_USED = 1 << 0
};

struct ECS {
    struct ComponentList lists[ECSCOMPONENT_LAST + 1];
    EntityId *ids;
    Bitmap used;
    size_t capacity;
    EntityId next_entity_id;
    struct World *world;
};

void _ecs_register_internal(
    enum ECSComponent id, size_t component_size,
    struct ECS *ecs, union ECSSystem system);

void _ecs_add_internal(struct Entity entity, enum ECSComponent component_id, void *value);

#define _ecs_add3(e, c, v) ({ __typeof__(v) _v = (v); _ecs_add_internal((e), (c), &_v); })
#define _ecs_add2(e, c) _ecs_add_internal((e), (c), NULL)

#define _ecs_add_overload(_1,_2,_3,NAME,...) NAME
#define ecs_add(...) _ecs_add_overload(__VA_ARGS__, _ecs_add3, _ecs_add2)(__VA_ARGS__)

void ecs_event(struct ECS *self, enum ECSEvent event);
struct Entity ecs_new(struct ECS *self);
void ecs_delete(struct ECS *self, struct Entity entity);
void ecs_remove(struct Entity entity, enum ECSComponent component);
bool ecs_has(struct Entity entity, enum ECSComponent component);
void *ecs_get(struct Entity entity, enum ECSComponent component);
void ecs_init(struct ECS *self, struct World *world);
void ecs_destroy(struct ECS *self);

#endif