#ifndef ECSTYPES_H
#define ECSTYPES_H

#include "../util/util.h"

#define ENTITY_NONE 0

typedef u64 EntityId;

struct Entity {
    EntityId id;
    u64 index;
    struct ECS *ecs;
};

#endif