#include "include/light.h"

#define QUEUE_SIZE 32768 

#define ENQUEUE(q, e)\
    assert(((q)->size + 1) < QUEUE_SIZE);\
    ((q)->elements[(q)->size++] = (e));

#define DEQUEUE(q) ((q)->elements[--(q)->size])

struct LightNode {
    ivec3s pos;
    u16 value;
};

struct LightQueue {
    struct LightNode elements[QUEUE_SIZE];
    size_t size;
};

static void add_propagate(struct World *world, struct LightQueue *queue, u16 mask, u16 offset) {
    while (queue->size != 0) {
        struct LightNode node = DEQUEUE(queue);
        u16 light = world_get_light(world, node.pos);

        for (enum Direction d = 0; d < 6; d++) {
            ivec3s n_pos = glms_ivec3_add(node.pos, DIR2IVEC3S(d));
            u64 n_data = world_get_data(world, n_pos);
            struct Block n_block = BLOCKS[chunk_data_to_block(n_data)];
            u16 n_light = chunk_data_to_light(n_data);

            if (n_block.is_transparent(world, n_pos) && (
                    LIGHT_R(n_light) + 1 < LIGHT_R(light) ||
                    LIGHT_G(n_light) + 1 < LIGHT_G(light) ||
                    LIGHT_B(n_light) + 1 < LIGHT_B(light) ||
                    LIGHT_I(n_light) + 1 < LIGHT_I(light))) {
                u16 result = 0;
                result = LIGHT_SET_R(result, max(LIGHT_R(n_light), LIGHT_R(light) - 1));
                result = LIGHT_SET_G(result, max(LIGHT_G(n_light), LIGHT_G(light) - 1));
                result = LIGHT_SET_B(result, max(LIGHT_B(n_light), LIGHT_B(light) - 1));
                result = LIGHT_SET_I(result, max(LIGHT_I(n_light), LIGHT_I(light) - 1));
                world_set_light(world, n_pos, result);
                ENQUEUE(queue, ((struct LightNode) { .pos = n_pos }));
            }
        }
    }
}

static void remove_propagate(struct World *world, struct LightQueue *queue, struct LightQueue *prop_queue) {
    while (queue->size != 0) {
        struct LightNode node = DEQUEUE(queue);
        u16 light = node.value;

        for (enum Direction d = 0; d < 6; d++) {
            ivec3s n_pos = glms_ivec3_add(node.pos, DIR2IVEC3S(d));
            u16 n_light = world_get_light(world, n_pos);

            if ((LIGHT_R(n_light) != 0 && LIGHT_R(n_light) < LIGHT_R(light)) ||
                (LIGHT_G(n_light) != 0 && LIGHT_G(n_light) < LIGHT_G(light)) ||
                (LIGHT_B(n_light) != 0 && LIGHT_B(n_light) < LIGHT_B(light)) ||
                (LIGHT_I(n_light) != 0 && LIGHT_I(n_light) < LIGHT_I(light))) {
                world_set_light(world, n_pos, 0);
                ENQUEUE(queue, ((struct LightNode) { .pos = n_pos, .value = n_light }));
            } else if (LIGHT_R(n_light) >= LIGHT_R(light) ||
                LIGHT_G(n_light) >= LIGHT_G(light) || 
                LIGHT_B(n_light) >= LIGHT_B(light) || 
                LIGHT_I(n_light) >= LIGHT_I(light)) {
                ENQUEUE(prop_queue, ((struct LightNode) { .pos = n_pos }));
            }
        }
    }
}

void light_add(struct World *world, ivec3s pos, u16 light) {
    struct LightQueue queue = { .size = 0 };

    if (!BLOCKS[world_get_block(world, pos)].is_transparent(world, pos)) {
        return;
    }

    world_set_light(world, pos, light);
    ENQUEUE(&queue, ((struct LightNode) { .pos = pos }));
    add_propagate(world, &queue);
}

void light_remove(struct World *world, ivec3s pos) {
    struct LightQueue
        queue = { .size = 0 },
        prop_queue = { .size = 0 };

    ENQUEUE(&queue, ((struct LightNode) { .pos = pos, .value = world_get_light(world, pos) }));
    world_set_light(world, pos, 0);
    remove_propagate(world, &queue, &prop_queue);
    add_propagate(world, &prop_queue);
}