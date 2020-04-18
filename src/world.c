#include "include/world.h"

#define world_foreach(_w, _cname)\
    struct Chunk *_cname;\
    for (size_t i = 0; i < ((_w)->chunks_size * (_w)->chunks_size) &&\
        (_cname = (_w)->chunks[i]) != (void *) INT64_MAX;\
        i++)

// chunk offset -> world array index
size_t world_chunk_index(struct World *self, ivec3s offset) {
    ivec3s p = glms_ivec3_sub(offset, self->chunks_origin);
    return p.z * self->chunks_size + p.x;
}

// world array index -> chunk offset
ivec3s world_chunk_offset(struct World *self, size_t i) {
    return glms_ivec3_add(
        self->chunks_origin,
        (ivec3s) {{ i % self->chunks_size, 0, i / self->chunks_size }}
    );
}

// block position -> chunk offset
ivec3s world_pos_to_offset(ivec3s pos) {
    return (ivec3s) {{
        (s32) floorf(pos.x / CHUNK_SIZE_F.x),
        0,
        (s32) floorf(pos.z / CHUNK_SIZE_F.z)
    }};
}

// float pos -> block pos
ivec3s world_pos_to_block(vec3s pos) {
    return (ivec3s) {{
        (s32) floorf(pos.x),
        (s32) floorf(pos.y),
        (s32) floorf(pos.z)
    }};
}

// world position -> chunk position
ivec3s world_pos_to_chunk_pos(ivec3s pos) {
    // ((pos % size) + size) % size
    return glms_ivec3_mod(glms_ivec3_add(glms_ivec3_mod(pos, CHUNK_SIZE), CHUNK_SIZE), CHUNK_SIZE);
}

bool world_chunk_in_bounds(struct World *self, ivec3s offset) {
    ivec3s p = glms_ivec3_sub(offset, self->chunks_origin);
    return p.x >= 0 && p.z >= 0 &&
        p.x < (s32) self->chunks_size && p.z < (s32) self->chunks_size;
}

struct Chunk *world_get_chunk(struct World *self, ivec3s offset) {
    if (!world_chunk_in_bounds(self, offset)) {
        return NULL;
    } else {
        return self->chunks[world_chunk_index(self, offset)];
    }
}

bool world_contains_chunk(struct World *self, ivec3s offset) {
    return world_get_chunk(self, offset) != NULL;
}

bool world_in_bounds(struct World *self, ivec3s pos) {
    return world_chunk_in_bounds(self, world_pos_to_offset(pos));
}

bool world_contains(struct World *self, ivec3s pos) {
    return world_contains_chunk(self, world_pos_to_offset(pos));
}

void world_load_chunk(struct World *self, ivec3s offset) {
    assert(!world_contains_chunk(self, offset));

    struct Chunk *chunk = malloc(sizeof(struct Chunk));
    chunk_init(chunk, self, offset);
    worldgen_generate(chunk);
    self->chunks[world_chunk_index(self, offset)] = chunk;
}

void world_init(struct World *self) {
    player_init(&self->player, self);
    self->chunks_size = 16;
    self->chunks = calloc(1, self->chunks_size * self->chunks_size * sizeof(struct Chunk *));
    world_set_center(self, GLMS_IVEC3_ZERO);
}

void world_destroy(struct World *self) {
    player_destroy(&self->player);

    world_foreach(self, chunk) {
        if (chunk != NULL) {
            chunk_destroy(chunk);
            free(chunk);
        }
    }

    free(self->chunks);
}

void world_set_data(struct World *self, ivec3s pos, u32 data) {
    ivec3s offset = world_pos_to_offset(pos);
    if (world_contains_chunk(self, offset)) {
        chunk_set_data(world_get_chunk(self, offset), world_pos_to_chunk_pos(pos), data);
    }
}

u32 world_get_data(struct World *self, ivec3s pos) {
    ivec3s offset = world_pos_to_offset(pos);
    if (pos.y >= 0 && pos.y < CHUNK_SIZE.y && world_contains_chunk(self, offset)) {
        return chunk_get_data(world_get_chunk(self, offset), world_pos_to_chunk_pos(pos));
    }
    return 0;
}

// Attempt to load any NULL chunks
static void load_empty_chunks(struct World *self) {
    for (size_t i = 0; i < (self->chunks_size * self->chunks_size); i++) {
        // TODO: rate limit this per-frame
        if (self->chunks[i] == NULL) {
            world_load_chunk(self, world_chunk_offset(self, i));
        }
    }
}

// Centers the world's loaded chunks around the specified block position
void world_set_center(struct World *self, ivec3s center_pos) {
    ivec3s new_offset = world_pos_to_offset(center_pos);
    ivec3s new_origin = glms_ivec3_sub(new_offset, (ivec3s) {{ self->chunks_size / 2, 0, self->chunks_size / 2 }});

    if (!memcmp(&new_origin, &self->chunks_origin, sizeof(ivec3s))) {
        // Do nothing if the center chunk hasn't moved
        return;
    }

    // Re-center
    self->center_offset = new_offset;
    self->chunks_origin = new_origin;

    size_t n_chunks = self->chunks_size * self->chunks_size;

    // Backup current chunks
    struct Chunk *old[n_chunks];
    memcpy(old, self->chunks, n_chunks * sizeof(struct Chunk *));

    // Set world to all unloaded chunks initially
    memset(self->chunks, 0, n_chunks * sizeof(struct Chunk *));

    // Place old chunks in positions of new chunk array, destroy if they are out of bounds now
    for (size_t i = 0; i < n_chunks; i++) {
        struct Chunk *c = old[i];
        if (c == NULL) {
            continue;
        } else if (world_chunk_in_bounds(self, c->offset)) {
            self->chunks[world_chunk_index(self, c->offset)] = c;
        } else {
            chunk_destroy(c);
            free(c);
        }
    }

    load_empty_chunks(self);
}

void world_render(struct World *self) {
    world_foreach(self, chunk) {
        if (chunk != NULL) {
            chunk_render(chunk);
        }
    }

    player_render(&self->player);
}

void world_update(struct World *self) {
    world_foreach(self, chunk) {
        if (chunk != NULL) {
            chunk_update(chunk);
        }
    }
    
    player_update(&self->player);
}

void world_tick(struct World *self) {
    world_foreach(self, chunk) {
        if (chunk != NULL) {
            chunk_tick(chunk);
        }
    }
 
    player_tick(&self->player);
}