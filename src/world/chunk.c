#include "../state.h"
#include "chunk.h"
#include "world.h"
#include "light.h"

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset) {
    memset(self, 0, sizeof(struct Chunk));
    self->world = world;
    self->offset = offset;
    self->position = glms_ivec3_mul(offset, CHUNK_SIZE);
    self->data = calloc(1, CHUNK_VOLUME * sizeof(u64));
    self->mesh = chunkmesh_create(self);
}

void chunk_destroy(struct Chunk *self) {
    free(self->data);
    chunkmesh_destroy(self->mesh);
}

// returns the chunks that border the specified chunk position
static void chunk_get_bordering_chunks(struct Chunk *self, ivec3s pos, struct Chunk *dest[6]) {
    size_t i = 0;

    if (pos.x == 0) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ -1, 0, 0 }}));
    }

    if (pos.y == 0) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 0, -1, 0 }}));
    }

    if (pos.z == 0) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 0, 0, -1 }}));
    }

    if (pos.x == (CHUNK_SIZE.x - 1)) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 1, 0, 0 }}));
    }

    if (pos.x == (CHUNK_SIZE.y - 1)) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 0, 1, 0 }}));
    }

    if (pos.z == (CHUNK_SIZE.z - 1)) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 0, 0, 1 }}));
    }
}

// MUST be run once a chunk has completed generating
void chunk_after_generate(struct Chunk *self) {
    chunk_heightmap_recalculate(self);
    light_apply(self);
}

// MUST be run after data inside of a chunk is modified
void chunk_on_modify(
    struct Chunk *self, ivec3s pos,
    u64 prev, u64 data) {
    self->mesh->flags.dirty = true;

    enum BlockId prev_block = chunk_data_to_block(prev),
        data_block = chunk_data_to_block(data);

    u32 prev_light = chunk_data_to_light(prev),
        light = chunk_data_to_light(data); 

    struct Block block_p = BLOCKS[prev_block], block = BLOCKS[data_block];

    if (data_block != prev_block) {
        ivec3s pos_w = glms_ivec3_add(self->position, pos);

        if (block_p.can_emit_light) {
            light_remove(self->world, pos_w);
        }

        if (block.can_emit_light) {
            torchlight_add(self->world, pos_w, block.get_torchlight(self->world, pos_w));
        }

        if (!self->flags.generating) {
            if (block.transparent) {
                world_heightmap_recalculate(self->world, (ivec2s) {{ pos_w.x, pos_w.z }});
                
                // propagate lighting through this block
                light_update(self->world, pos_w);
            } else {
                world_heightmap_update(self->world, pos_w);
                
                // remove light at this block
                light_remove(self->world, pos_w);
            }
        }

        self->count += (data_block == AIR ? -1 : 1);
    }

    self->flags.empty = self->count == 0;

    // mark any chunks that could have been affected as dirty
    if ((data_block != prev_block || light != prev_light)
            && chunk_on_bounds(pos)) {
        struct Chunk *neighbors[6] = { NULL };
        chunk_get_bordering_chunks(self, pos, neighbors);

        for (size_t i = 0; i < 6; i++) {
            if (neighbors[i] != NULL) {
                neighbors[i]->mesh->flags.dirty = true;
            }
        }
    }
}

void chunk_prepare(struct Chunk *self) {
    if (self->flags.empty) {
        return;
    }

    chunkmesh_prepare_render(self->mesh);
}

void chunk_render(struct Chunk *self, enum ChunkMeshPart part) {
    if (self->flags.empty) {
        return;
    }

    chunkmesh_render(self->mesh, part);
}

void chunk_update(struct Chunk *self) {
    // Depth sort the transparent mesh if
    // (1) the player is inside of this chunk and their block position changed
    // (2) the player has moved chunks AND this chunk is close
    struct PositionComponent *c_position = ecs_get(self->world->entity_view, C_POSITION);
    bool within_distance = glms_ivec3_norm(glms_ivec3_sub(self->offset, c_position->offset)) < 4;

    self->mesh->flags.depth_sort =
        (!ivec3scmp(self->offset, c_position->offset) && c_position->block_changed) ||
        (c_position->offset_changed && within_distance);

    // Persist depth sort data if the player is within depth sort distance of this chunk
    chunkmesh_set_persist(self->mesh, within_distance);
}

void chunk_tick(struct Chunk *self) {

}