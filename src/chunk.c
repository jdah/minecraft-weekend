#include "include/chunk.h"
#include "include/state.h"

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset) {
    memset(self, 0, sizeof(struct Chunk));
    self->world = world;
    self->offset = offset;
    self->position = glms_ivec3_mul(offset, CHUNK_SIZE);
    self->data = calloc(1, CHUNK_VOLUME * sizeof(u32));
    self->meshes.base = chunkmesh_create(self, false);
    self->meshes.transparent = chunkmesh_create(self, true);
}

void chunk_destroy(struct Chunk *self) {
    free(self->data);
    chunkmesh_destroy(self->meshes.base);
    chunkmesh_destroy(self->meshes.transparent);
}

// returns true if pos is within chunk boundaries
bool chunk_in_bounds(ivec3s pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 &&
        pos.x < CHUNK_SIZE.x && pos.y < CHUNK_SIZE.y && pos.z < CHUNK_SIZE.z;
}

// returns true if pos is on chunk boundaries (borders another chunk)
bool chunk_on_bounds(ivec3s pos) {
    return pos.x == 0 || pos.y == 0 || pos.z == 0 ||
        pos.x == (CHUNK_SIZE.x - 1) || pos.y == (CHUNK_SIZE.y - 1) || pos.z == (CHUNK_SIZE.z - 1);
}

// returns the chunks that border the specified chunk position
void chunk_get_bordering_chunks(struct Chunk *self, ivec3s pos, struct Chunk *dest[6]) {
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

void chunk_set_data(struct Chunk *self, ivec3s pos, u32 data) {
    assert(chunk_in_bounds(pos));

    const size_t index = chunk_pos_to_index(pos);
    u32 prev_data = self->data[index];
    self->data[index] = data;
    self->flags.dirty |= data != prev_data;

    if (data != prev_data) {
        self->count += data == AIR ? -1 : 1;
    }

    self->flags.empty = self->count == 0;

    // mark any chunks that could have been affected as dirty
    if (chunk_on_bounds(pos)) {
        struct Chunk *neighbors[6] = { NULL };
        chunk_get_bordering_chunks(self, pos, neighbors);

        for (size_t i = 0; i < 6; i++) {
            if (neighbors[i] != NULL) {
                neighbors[i]->flags.dirty = true;
            }
        }
    }
}

u32 chunk_get_data(struct Chunk *self, ivec3s pos) {
    assert(chunk_in_bounds(pos));
    return self->data[chunk_pos_to_index(pos)];
}

void chunk_render(struct Chunk *self) {
    if (self->flags.empty) {
        return;
    }

    if (self->world->throttles.mesh.count < self->world->throttles.mesh.max) {
        if (self->flags.dirty) {
            chunk_mesh(self, FULL);
            self->flags.dirty = false;
            self->flags.depth_sort = false;
            self->world->throttles.mesh.count++;
        } else if (self->flags.depth_sort) {
            chunk_depth_sort(self);
            self->flags.depth_sort = false;
            self->world->throttles.mesh.count++;
        }
    }

    chunkmesh_render(self->meshes.base);
    chunkmesh_render(self->meshes.transparent);
}

void chunk_update(struct Chunk *self) {
    // Depth sort the transparent mesh if
    // (1) the player is inside of this chunk and their block position changed
    // (2) the player has moved chunks AND this chunk is close (within 1 chunk distance)
    struct EntityPlayer *player = &self->world->player;
    bool within_distance = glms_ivec3_norm(glms_ivec3_sub(self->offset, player->offset)) < 4;

    self->flags.depth_sort =
        (!ivec3scmp(self->offset, player->offset) && player->block_pos_changed) ||
        (player->offset_changed && within_distance);

    // Persist depth sort data if the player is within depth sort distance of this chunk
    chunkmesh_set_persist(self->meshes.transparent, within_distance);
}

void chunk_tick(struct Chunk *self) {

}