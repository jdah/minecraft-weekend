#include "include/world.h"

#define world_foreach(_w, _cname)\
    struct Chunk *_cname;\
    for (size_t i = 0; i < ((_w)->chunks_size * (_w)->chunks_size) &&\
        (_cname = (_w)->chunks[i]) != (void *) INT64_MAX;\
        i++)

// back-to-front ordering pointer comparator
static int _btf_pcmp(ivec3s *center, const ivec3s **a, const ivec3s **b) {
    return -(glms_ivec3_norm2(glms_ivec3_sub(*center, **a)) - glms_ivec3_norm2(glms_ivec3_sub(*center, **b)));
}

// front-to-back ordering pointer comparator
static int _ftb_pcmp(ivec3s *center, const ivec3s **a, const ivec3s **b) {
    return glms_ivec3_norm2(glms_ivec3_sub(*center, **a)) - glms_ivec3_norm2(glms_ivec3_sub(*center, **b));
}

// back-to-front ordering comparator
static int _btf_cmp(ivec3s *center, const ivec3s *a, const ivec3s *b) {
    return -(glms_ivec3_norm2(glms_ivec3_sub(*center, *a)) - glms_ivec3_norm2(glms_ivec3_sub(*center, *b)));
}

// front-to-back ordering comparator
static int _ftb_cmp(ivec3s *center, const ivec3s *a, const ivec3s *b) {
    return -(glms_ivec3_norm2(glms_ivec3_sub(*center, *b)) - glms_ivec3_norm2(glms_ivec3_sub(*center, *a)));
}

// _cmp is comparison function for qsort_r
// _v0 is 'n'
// _v1 is counter
// _v2 is ivec3s* offsets array
// TODO: qsort_r is not portable
#define _world_foreach_cmp_impl(_w, _cname, _cmp, _v0, _v1, _v2)\
    size_t _v0 = 0, _v1 = 0;\
    ivec3s *_v2[_w->chunks_size * _w->chunks_size];\
    for (_v1 = 0; _v1 < (_w->chunks_size * _w->chunks_size); _v1++)\
        { if (_w->chunks[_v1] != NULL) _v2[_v0++] = &_w->chunks[_v1]->offset; }\
    qsort_r(_v2, _v0, sizeof(ivec3s*), &_w->center_offset, (int (*)(void*, const void*, const void*)) _cmp);\
    struct Chunk *_cname;\
    for (size_t i = 0; i < _v0 &&\
        (_cname = (_w)->chunks[world_chunk_index(_w, *_v2[i])]) != (void *) INT64_MAX;\
        i++)

#define _world_foreach_offset_cmp_impl(_w, _iname, _oname, _cmp, _v0, _v1, _v2)\
    size_t _v0 = (_w->chunks_size * _w->chunks_size), _v1 = 0, _iname = 0;\
    ivec3s _v2[_v0];\
    for (_v1 = 0; _v1 < _v0; _v1++)\
        { _v2[_v1] = world_chunk_offset(_w, _v1); }\
    qsort_r(_v2, _v0, sizeof(ivec3s), &_w->center_offset, (int (*)(void*, const void*, const void*)) _cmp);\
    ivec3s _oname;\
    for (_v1 = 0; _v1 < _v0 &&\
        ((_iname = world_chunk_index(_w, _v2[_v1])) != (size_t) - 1 &&\
        (_oname.x = _v2[_v1].x) != INT32_MIN &&\
        (_oname.y = _v2[_v1].y) != INT32_MIN &&\
        (_oname.z = _v2[_v1].z) != INT32_MIN);\
        _v1++)

#define CONCAT_IMPL(x, y) x ## y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

// iterate chunks from the borders in (back to front)
#define world_foreach_btf(_w, _cname)\
    _world_foreach_cmp_impl(_w, _cname, _btf_pcmp, CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__))
    
// iterate chunks from the center out (front to back)
#define world_foreach_ftb(_w, _cname)\
    _world_foreach_cmp_impl(_w, _cname, _ftb_pcmp, CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__))

// iterate OFFSETS from the borders in (back to front)
#define world_foreach_offset_btf(_w, _iname, _oname)\
    _world_foreach_offset_cmp_impl(_w, _iname, _oname, _btf_cmp, CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__))

// iterate OFFSETS from the center out (front to back)
#define world_foreach_offset_ftb(_w, _iname, _oname)\
    _world_foreach_offset_cmp_impl(_w, _iname, _oname, _ftb_cmp, CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__), CONCAT(v, __COUNTER__))

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
    memset(self, 0, sizeof(struct World));
    self->throttles.load.max = 2;
    self->throttles.mesh.max = 2;

    self->unloaded_data.capacity = 64;
    self->unloaded_data.list = malloc(self->unloaded_data.capacity * sizeof(struct WorldUnloadedData));

    player_init(&self->player, self);
    self->chunks_size = 20;
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

void world_append_unloaded_data(struct World *self, ivec3s pos, u32 data) {
    if (self->unloaded_data.size + 1 == self->unloaded_data.capacity) {
        self->unloaded_data.capacity *= 2;
        self->unloaded_data.list = realloc(
            self->unloaded_data.list,
            self->unloaded_data.capacity * sizeof(struct WorldUnloadedData));
    }

    self->unloaded_data.list[self->unloaded_data.size++] = (struct WorldUnloadedData) {
        .pos = pos,
        .data = data
    };
}

void world_remove_unloaded_data(struct World *self, size_t i) {
    assert(i >= 0 && i < self->unloaded_data.size);
    self->unloaded_data.size--;

    if (i != self->unloaded_data.size) {
        memmove(
            self->unloaded_data.list + (i + 0),
            self->unloaded_data.list + (i + 1),
            (self->unloaded_data.size - i) * sizeof(struct WorldUnloadedData));
    }
}

void world_set_data(struct World *self, ivec3s pos, u32 data) {
    ivec3s offset = world_pos_to_offset(pos);
    if (world_contains_chunk(self, offset)) {
        chunk_set_data(world_get_chunk(self, offset), world_pos_to_chunk_pos(pos), data);
    } else {
        world_append_unloaded_data(self, pos, data);
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
    // TODO: load from center out
    // world_foreach_offset_ftb(self, i, offset) {
    //     if (self->chunks[i] == NULL &&
    //         self->throttles.load.count < self->throttles.load.max) {
    //         world_load_chunk(self, world_chunk_offset(self, i));
    //         self->throttles.load.count++;
    //     }
    // }

    for (size_t i = 0; i < self->chunks_size * self->chunks_size; i++) {
        if (self->chunks[i] == NULL &&
            self->throttles.load.count < self->throttles.load.max) {
            world_load_chunk(self, world_chunk_offset(self, i));
            self->throttles.load.count++;
        }
    }
}

// Centers the world's loaded chunks around the specified block position
void world_set_center(struct World *self, ivec3s center_pos) {
    ivec3s new_offset = world_pos_to_offset(center_pos);
    ivec3s new_origin = glms_ivec3_sub(new_offset, (ivec3s) {{ (self->chunks_size / 2) - 1, 0, (self->chunks_size / 2) - 1 }});

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
    world_foreach_btf(self, c) {
        if (c != NULL) {
            chunk_render(c);

            // TODO: this is a hack to get roses to work, they need to emit the proper indices
            glDisable(GL_CULL_FACE);
            chunk_render_transparent(c);
            glEnable(GL_CULL_FACE);
        }
    }

    player_render(&self->player);
}

void world_update(struct World *self) {
    // Reset per-frame throttles
    self->throttles.load.count = 0;
    self->throttles.mesh.count = 0;

    load_empty_chunks(self);

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