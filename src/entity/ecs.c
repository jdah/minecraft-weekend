#include "ecs.h"

// TODO: real alignment, this is a silly hack
#define ECS_TAG_SIZE 16
#define ECS_TAG(_p) *((ECSTag *) (((void *) (_p)) - ECS_TAG_SIZE))
#define ECS_PTAG(_p) ((ECSTag *) (((void *) (_p)) - ECS_TAG_SIZE))
#define ECSCL_ELEMENT_SIZE(_plist) ((_plist)->component_size + ECS_TAG_SIZE)
#define ECSCL_GET(_plist, _i) ({\
        struct ComponentList *_pl = (_plist);\
        ((_pl)->components) + ((_i) * ECSCL_ELEMENT_SIZE(_pl)) + ECS_TAG_SIZE;\
    })

void _ecs_register_internal(
    enum ECSComponent id, size_t component_size,
    struct ECS *ecs, union ECSSystem system) {
    struct ComponentList list = {
        .component_size = component_size,
        .system = system
    };
    list.components = calloc(ecs->capacity, ECSCL_ELEMENT_SIZE(&list));
    ecs->lists[id] = list;
}

void ecs_event(struct ECS *self, enum ECSEvent event) {
    for (size_t i = 0; i <= ECSCOMPONENT_LAST; i++) {
        struct ComponentList *list = &self->lists[i];
        
        // get this component's subscriber for this event
        ECSSubscriber f = list->system.subscribers[event];
        if (f == NULL) {
            continue;
        }

        for (size_t j = 0; j < self->capacity; j++) {
            void *component = ECSCL_GET(list, j);
            if (ECS_TAG(component) & ECS_TAG_USED) {
                f(component, (struct Entity) { .id = self->ids[j], .index = j, .ecs = self });
            }
        }
    }
}

// create a new entity
struct Entity ecs_new(struct ECS *self) {
    // entity index
    size_t i;

    // search for an open id
    for (i = 0; i < self->capacity; i += 64) {
        if (((u64 *) self->used)[(i / 64)] != 0xFFFFFFFFFFFFFFFF) {
            break;
        }
    }

    if (i == self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity *= 2;

        // reallocate bitmap, clear new allocation
        self->used = bitmap_realloc(self->used, self->capacity);
        memset(
            ((void *) self->used) + (BITMAP_SIZE_TO_BYTES(old_capacity)),
            0, BITMAP_SIZE_TO_BYTES(self->capacity) - BITMAP_SIZE_TO_BYTES(old_capacity));
        
        // reallocate index -> ID map, clear new allocation
        self->ids = realloc(self->ids, self->capacity * sizeof(u64));
        memset(
            self->ids + old_capacity, 0,
            (self->capacity - old_capacity) * sizeof(u64));

        // reallocate component lists
        for (size_t j = 0; j <= ECSCOMPONENT_LAST; j++) {
            struct ComponentList *list = &self->lists[j];
            list->components = realloc(
                list->components,
                self->capacity * ECSCL_ELEMENT_SIZE(list));

            // initialize new allocation
            memset(
                list->components + (old_capacity * ECSCL_ELEMENT_SIZE(list)), 0,
                (self->capacity - old_capacity) * ECSCL_ELEMENT_SIZE(list)); 
        }
    } else {
        for (; i < self->capacity; i++) {
            if (!bitmap_get(self->used, i)) {
                break;
            }
        }
    }

    // mark this entity as used
    bitmap_set(self->used, i);

    EntityId id = self->next_entity_id++;
    self->ids[i] = id;

    return (struct Entity) {
        .id = id,
        .index = i,
        .ecs = self
    };
}

void ecs_delete(struct ECS *self, struct Entity entity) {
    assert(bitmap_get(self->used, entity.index));

    // remove components
    for (size_t j = 0; j <= ECSCOMPONENT_LAST; j++) {
        struct ComponentList *list = &self->lists[j];
        ECSSubscriber destroy = list->system.destroy;

        // mark this component as unused
        void *component = ECSCL_GET(list, entity.index);
        *ECS_PTAG(component) &= ~ECS_TAG_USED;

        // run destructor if non-null
        if (destroy != NULL) {
            destroy(component, entity);
        }
    }

    // mark this entity's index as unused
    bitmap_clr(self->used, entity.index);

    // remove this id map entry
    self->ids[entity.index] = ENTITY_NONE;
}

void _ecs_add_internal(struct Entity entity, enum ECSComponent component_id, void *value) {
    struct ComponentList *list = &entity.ecs->lists[component_id];
    ECSSubscriber init = list->system.init;
    void *component = ECSCL_GET(list, entity.index);

    // mark the component as used
    assert(!(ECS_TAG(component) & ECS_TAG_USED));
    *ECS_PTAG(component) |= ECS_TAG_USED;

    if (value != NULL) {
        memcpy(component, value, list->component_size);
    }

    // run the initializer if it is not null
    if (init != NULL) {
        init(component, entity);
    }
}

void ecs_remove(struct Entity entity, enum ECSComponent component_id) {
    struct ComponentList *list = &entity.ecs->lists[component_id];
    ECSSubscriber destroy = list->system.destroy;
    void *component = ECSCL_GET(list, entity.index);

    // mark the component as unused
    assert(ECS_TAG(component) & ECS_TAG_USED);
    *ECS_PTAG(component) &= ~ECS_TAG_USED;

    // run the destructor if it is not null
    if (destroy != NULL) {
        destroy(component, entity);
    }
}

bool ecs_has(struct Entity entity, enum ECSComponent component) {
    return ECS_TAG(ECSCL_GET(&entity.ecs->lists[component], entity.index)) & ECS_TAG_USED;
}

void *ecs_get(struct Entity entity, enum ECSComponent component) {
    assert(ecs_has(entity, component));
    return ECSCL_GET(&entity.ecs->lists[component], entity.index);
}

void ecs_init(struct ECS *self, struct World *world) {
    self->capacity = 64;
    self->ids = calloc(self->capacity, sizeof(EntityId));
    self->used = bitmap_calloc(self->capacity);
    self->next_entity_id = 1;
    self->world = world;

    // defined in ecs.h
    _ecs_init_internal(self);
}

void ecs_destroy(struct ECS *self) {
    free(self->used);
    free(self->ids);

    for (size_t i = 0; i <= ECSCOMPONENT_LAST; i++) {
        free(self->lists[i].components);
    }
}