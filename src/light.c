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

static void add_propagate(
    struct World *world, struct LightQueue *queue,
    u16 mask, u16 offset) {
    while (queue->size != 0) {
        struct LightNode node = DEQUEUE(queue);
        u16 light = world_get_light(world, node.pos);

        for (enum Direction d = 0; d < 6; d++) {
            ivec3s n_pos = glms_ivec3_add(node.pos, DIR2IVEC3S(d));
            u64 n_data = world_get_data(world, n_pos);
            struct Block n_block = BLOCKS[chunk_data_to_block(n_data)];
            u16 n_light = chunk_data_to_light(n_data);

            if (n_block.is_transparent(world, n_pos) &&
                ((n_light & mask) >> offset) + 1 < ((light & mask) >> offset)) {
                world_set_light(
                    world, n_pos,
                    (n_light & ~mask) | ((((light & mask) >> offset) - 1) << offset)
                );
                ENQUEUE(queue, ((struct LightNode) { .pos = n_pos }));
            }
        }
    }
}

static void remove_propagate(
    struct World *world, struct LightQueue *queue, struct LightQueue *prop_queue,
    u16 mask, u16 offset) {
    while (queue->size != 0) {
        struct LightNode node = DEQUEUE(queue);
        u16 value = node.value;

        for (enum Direction d = 0; d < 6; d++) {
            ivec3s n_pos = glms_ivec3_add(node.pos, DIR2IVEC3S(d));
            u16 n_light = world_get_light(world, n_pos),
                n_value = (n_light & mask) >> offset;

            if ((n_light & mask) != 0 && n_value < value) {
                world_set_light(world, n_pos, n_light & ~mask);
                ENQUEUE(queue, ((struct LightNode) { .pos = n_pos, .value = n_value }));
            } else if (n_value >= value) {
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

    for (size_t i = 0; i < 4; i++) {
        queue.size = 0;
        ENQUEUE(&queue, ((struct LightNode) { .pos = pos }));
        add_propagate(world, &queue, 0xF << (i * 4), i * 4);
    }
}

void light_remove(struct World *world, ivec3s pos) {
    struct LightQueue
        queue = { .size = 0 },
        prop_queue = { .size = 0 };

    u16 light = world_get_light(world, pos); 
    world_set_light(world, pos, 0);

    for (size_t i = 0; i < 4; i++) {
        u16 mask = 0xF << (i * 4), offset = i * 4;
        queue.size = 0;
        prop_queue.size = 0;

        ENQUEUE(&queue, ((struct LightNode) { .pos = pos, .value = (light & mask) >> offset }));
        remove_propagate(world, &queue, &prop_queue, mask, offset);
        add_propagate(world, &prop_queue, mask, offset);
    }
}