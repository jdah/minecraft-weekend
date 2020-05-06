#ifndef ECS_H
#define ECS_H

#include "../util/util.h"

// forward declarations
struct World;

#define ecs_register(_id, _C, _ecs, _system) _ecs_register_internal((_id), sizeof(_C), (_ecs), (_system))

#define ENTITY_NONE 0

typedef u64 EntityId;
typedef u64 ECSTag;

#define ECSCOMPONENT_LAST C_POSITION
enum ECSComponent {
    C_POSITION = 0
};

#define ECSEVENT_LAST ECS_TICK
enum ECSEvent {
    ECS_INIT = 0, ECS_DESTROY, ECS_RENDER, ECS_UPDATE, ECS_TICK
};

struct Entity {
    EntityId id;
    u64 index;
    struct ECS *ecs;
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

void ecs_event(struct ECS *self, enum ECSEvent event);
struct Entity ecs_new(struct ECS *self);
void ecs_delete(struct ECS *self, struct Entity entity);
void ecs_add(struct Entity entity, enum ECSComponent component);
void ecs_remove(struct Entity entity, enum ECSComponent component);
bool ecs_has(struct Entity entity, enum ECSComponent component);
void *ecs_get(struct Entity entity, enum ECSComponent component);
void ecs_init(struct ECS *self, struct World *world);

#endif