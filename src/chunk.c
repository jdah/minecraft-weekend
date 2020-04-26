#include "include/chunk.h"
#include "include/state.h"

#define chunk_foreach(_pname)\
    ivec3s _pname = GLMS_IVEC3_ZERO_INIT;\
    for (s32 x = 0; x < CHUNK_SIZE.x; x++)\
        for (s32 y = 0; y < CHUNK_SIZE.y; y++)\
            for (s32 z = 0;\
                z < CHUNK_SIZE.z &&\
                ((_pname.x = x) != INT32_MAX) &&\
                ((_pname.y = y) != INT32_MAX) &&\
                ((_pname.z = z) != INT32_MAX);\
                z++)

#define chunk_pos_to_index(p) (p.x * CHUNK_SIZE.y * CHUNK_SIZE.z + p.y * CHUNK_SIZE.z + p.z)

const u16 FACE_INDICES[] = {1, 0, 3, 1, 3, 2};
const u16 UNIQUE_INDICES[] = {1, 0, 5, 2};
const u16 CUBE_INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};

const u16 SPRITE_INDICES[] = {
    3, 0, 5, 3, 5, 6,
    4, 2, 1, 4, 2, 7
};

const f32 CUBE_VERTICES[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,

    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
};

const f32 CUBE_UVS[] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1
};

#define DATA_BUFFER_SIZE (16 * 256 * 16) * 8 * 5 * sizeof(f32)
#define INDICES_BUFFER_SIZE (16 * 256 * 16) * 36 * sizeof(u16)
#define FACES_BUFFER_SIZE (16 * 256 * 16) * 96 * sizeof(u16)

// global mesh data buffers
// TODO: if threading is introduced, make these per-thread
struct {
    f32 data[DATA_BUFFER_SIZE];
    u16 indices[INDICES_BUFFER_SIZE];
    struct Face faces[FACES_BUFFER_SIZE];
} global_buffers[3];

void mesh_init(struct Mesh *self, struct Chunk *chunk) {
    memset(self, 0, sizeof(struct Mesh));
    self->chunk = chunk;
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, false);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);

    // Set buffer capacities
    struct MeshBuffer *buffers[3] = {
        &self->data, &self->indices, &self->faces
    };

    for (size_t i = 0; i < 3; i++) {
        buffers[i]->capacity = ((size_t[3]) {
            DATA_BUFFER_SIZE,
            INDICES_BUFFER_SIZE,
            FACES_BUFFER_SIZE
        })[i];
    }
}

void mesh_destroy(struct Mesh *self) {
    vao_destroy(self->vao);
    vbo_destroy(self->vbo);
    vbo_destroy(self->ibo);
}

static int _depth_sort_cmp(const struct Face *a, const struct Face *b) {
    return (int) -sign(a->distance - b->distance);
}

void mesh_depth_sort(struct Mesh *self, vec3s center) {
    // calulate face distance
    for (size_t i = 0; i < self->faces.count; i++) {
        struct Face *face = &((struct Face*) self->faces.data)[i];
        face->distance = glms_vec3_norm2(glms_vec3_sub(center, face->position));
    }

    // sort faces
    quicksort(
        self->faces.data, self->faces.count, sizeof(struct Face),
        (int (*)(const void*, const void*)) _depth_sort_cmp);

    // reorder indices according to face ordering
    u16 *t = global_buffers[2].indices;
    for (size_t i = 0; i < self->faces.count; i++) {
        struct Face *face = &((struct Face*) self->faces.data)[i];
        memcpy(&t[i * 6], &((u16*) self->indices.data)[face->indices_base], 6 * sizeof(u16));
        face->indices_base = i * 6;
    }

    memcpy(self->indices.data, t, self->indices.count * sizeof(u16));
}

void mesh_prepare(struct Mesh *self, size_t global_buffers_index) {
    self->vertex_count = 0;

    struct MeshBuffer *buffers[3] = {
        &self->data, &self->indices, &self->faces
    };

    for (size_t i = 0; i < 3; i++) {
        struct MeshBuffer *buffer = buffers[i];
        buffer->count = 0;
        buffer->index = 0;
        buffer->data = ((void*[3]) {
            global_buffers[global_buffers_index].data,
            global_buffers[global_buffers_index].indices,
            global_buffers[global_buffers_index].faces,
        })[i];
    }
}

void mesh_finalize(struct Mesh *self, bool depth_sort) {
    struct MeshBuffer *buffers[3] = {
        &self->data, &self->indices, &self->faces
    };

    // Flip each buffer
    for (size_t i = 0; i < 3; i++) {
        struct MeshBuffer *buffer = buffers[i];
        buffer->count = buffer->index;
        buffer->index = 0;
    }

    vbo_buffer(self->vbo, self->data.data, 0, (self->data.count) * sizeof(f32));

    // sort indices before buffering if this mesh requires depth sorting
    if (depth_sort) {
        mesh_depth_sort(self, self->chunk->world->player.camera.position);
    }

    vbo_buffer(self->ibo, self->indices.data, 0, (self->indices.count) * sizeof(u16));
}
 
void mesh_render(struct Mesh *self) {
    shader_bind(state.shader);
    shader_uniform_camera(state.shader, state.world.player.camera);
    shader_uniform_mat4(state.shader, "m", glms_translate(glms_mat4_identity(), IVEC3S2V(self->chunk->position)));
    shader_uniform_texture2D(state.shader, "tex", state.block_atlas.atlas.texture, 0);

    const size_t vertex_size = 8 * sizeof(f32);
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, vertex_size, 0 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, vertex_size, 3 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 2, 3, GL_FLOAT, vertex_size, 5 * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, self->indices.count, GL_UNSIGNED_SHORT, NULL);
}

static void emit_sprite(
    struct Mesh *mesh, vec3s position, vec2s uv_offset, vec2s uv_unit) {
    // Emitting 2 transparent faces
    for (size_t i = 0; i < 2; i++) {
        struct Face face = {
            .indices_base = mesh->indices.index + (i * 6),
            .position = position
        };

        memcpy(
            ((struct Face *) mesh->faces.data) + (mesh->faces.index),
            &face, sizeof(struct Face));
        mesh->faces.index++;
    }

    // Emit vertices
    for (size_t i = 0; i < 8;  i++) {
        const f32 *vertex = &CUBE_VERTICES[i * 3];
        ((f32*) mesh->data.data)[mesh->data.index++] = position.x + vertex[0];
        ((f32*) mesh->data.data)[mesh->data.index++] = position.y + vertex[1];
        ((f32*) mesh->data.data)[mesh->data.index++] = position.z + vertex[2];
        ((f32*) mesh->data.data)[mesh->data.index++] = uv_offset.x + (uv_unit.x * CUBE_UVS[((i % 4) * 2) + 0]);
        ((f32*) mesh->data.data)[mesh->data.index++] = uv_offset.y + (uv_unit.y * CUBE_UVS[((i % 4) * 2) + 1]);

        // color
        for (size_t j = 0; j < 3; j++) {
            ((f32*) mesh->data.data)[mesh->data.index++] = 1.0f;
        } 
    }

    // Emit indices
    for (size_t i = 0; i < 12; i++) {
        ((u16*) mesh->indices.data)[mesh->indices.index++] = mesh->vertex_count + SPRITE_INDICES[i];
    }

    // Emitted 8 vertices
    mesh->vertex_count += 8; 
}

static void emit_face(
    struct Mesh *mesh, vec3s position, enum Direction direction, 
    vec2s uv_offset, vec2s uv_unit, bool transparent, bool shorten_y) {
    // add this face into the face buffer if it's transparent
    if (transparent) {
        struct Face face = {
            .indices_base = mesh->indices.index,
            .position = position
        };

        memcpy(
            ((struct Face *) mesh->faces.data) + (mesh->faces.index),
            &face, sizeof(struct Face));
        mesh->faces.index++;
    }

    // Emit vertices
    for (size_t i = 0; i < 4;  i++) {
        const f32 *vertex = &CUBE_VERTICES[CUBE_INDICES[(direction * 6) + UNIQUE_INDICES[i]] * 3];
        ((f32*) mesh->data.data)[mesh->data.index++] = position.x + vertex[0];
        ((f32*) mesh->data.data)[mesh->data.index++] = position.y + ((shorten_y ? 0.9f : 1.0f) * vertex[1]);
        ((f32*) mesh->data.data)[mesh->data.index++] = position.z + vertex[2];
        ((f32*) mesh->data.data)[mesh->data.index++] = uv_offset.x + (uv_unit.x * CUBE_UVS[(i * 2) + 0]);
        ((f32*) mesh->data.data)[mesh->data.index++] = uv_offset.y + (uv_unit.y * CUBE_UVS[(i * 2) + 1]);

        // TODO: real lighting
        // Vary color according to face direction
        f32 color;
        if (transparent) {
            color = 1.0f;
        } else {
            switch (direction) {
                case UP:
                    color = 1.0f;
                    break;
                case NORTH:
                case SOUTH:
                    color = 0.86f;
                    break;
                case EAST:
                case WEST:
                    color = 0.8f;
                    break;
                case DOWN:
                    color = 0.6f;
                    break;
            }
        }

        for (size_t j = 0; j < 3; j++) {
            ((f32*) mesh->data.data)[mesh->data.index++] = color;
        } 
    }

    // Emit indices
    for (size_t i = 0; i < 6; i++) {
        ((u16*) mesh->indices.data)[mesh->indices.index++] = mesh->vertex_count + FACE_INDICES[i];
    }

    // Emitted 4 more vertices, bump the vertex count
    mesh->vertex_count += 4;
}

void chunk_init(struct Chunk *self, struct World *world, ivec3s offset) {
    memset(self, 0, sizeof(struct Chunk));
    self->world = world;
    self->offset = offset;
    self->position = glms_ivec3_mul(offset, CHUNK_SIZE);
    self->data = calloc(1, CHUNK_VOLUME * sizeof(u32));
    mesh_init(&self->meshes.base, self);
    mesh_init(&self->meshes.transparent, self);
}

void chunk_destroy(struct Chunk *self) {
    free(self->data);
    mesh_destroy(&self->meshes.base);
    mesh_destroy(&self->meshes.transparent);
}

// returns true if pos is within chunk boundaries
bool chunk_in_bounds(ivec3s pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 &&
        pos.x < CHUNK_SIZE.x && pos.y < CHUNK_SIZE.y && pos.z < CHUNK_SIZE.z;
}

// returns true if pos is on chunk boundaries (borders another chunk)
bool chunk_on_bounds(ivec3s pos) {
    return pos.x == 0 || pos.z == 0 || pos.x == (CHUNK_SIZE.x - 1) || pos.z == (CHUNK_SIZE.z - 1);
}

// returns the chunks that border the specified chunk position
void chunk_get_bordering_chunks(struct Chunk *self, ivec3s pos, struct Chunk *dest[2]) {
    size_t i = 0;

    if (pos.x == 0) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ -1, 0, 0 }}));
    }

    if (pos.z == 0) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 0, 0, -1 }}));
    }

    if (pos.x == (CHUNK_SIZE.x - 1)) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 1, 0, 0 }}));
    } 

    if (pos.z == (CHUNK_SIZE.z - 1)) {
        dest[i++] = world_get_chunk(self->world, glms_ivec3_add(self->offset, (ivec3s) {{ 0, 0, 1 }}));
    }
}

void chunk_set_data(struct Chunk *self, ivec3s pos, u32 data) {
    assert(chunk_in_bounds(pos));
    self->data[chunk_pos_to_index(pos)] = data;
    self->dirty = true;

    // mark any chunks that could have been affected as dirty
    if (chunk_on_bounds(pos)) {
        struct Chunk *neighbors[2] = { NULL, NULL };
        chunk_get_bordering_chunks(self, pos, neighbors);

        for (size_t i = 0; i < 2; i++) {
            if (neighbors[i] != NULL) {
                neighbors[i]->dirty = true;
            }
        }
    }
}

u32 chunk_get_data(struct Chunk *self, ivec3s pos) {
    assert(chunk_in_bounds(pos));
    return self->data[chunk_pos_to_index(pos)];
}

enum MeshPass {
    FULL,
    TRANSPARENCY
};

static void mesh(struct Chunk *self, enum MeshPass pass) {
    if (pass == FULL) {
        mesh_prepare(&self->meshes.base, 0);
    }

    mesh_prepare(&self->meshes.transparent, 1);

    chunk_foreach(pos) {
        vec3s fpos = IVEC3S2V(pos);
        ivec3s wpos = glms_ivec3_add(pos, self->position);

        u32 data = self->data[chunk_pos_to_index(pos)];
        if(data != 0) {
            struct Block block = BLOCKS[data], neighbor_block;
            bool transparent = block.is_transparent(self->world, wpos);
            
            if (block.is_sprite()) {
                emit_sprite(
                    &self->meshes.transparent, fpos,
                    atlas_offset(state.block_atlas.atlas, BLOCKS[data].get_texture_location(self->world, wpos, NORTH)),
                    state.block_atlas.atlas.sprite_unit);
            } else {
                bool shorten_y = false;

                if (block.is_liquid()) {
                    ivec3s up = (ivec3s) {{ pos.x, pos.y + 1, pos.z }};

                    if (chunk_in_bounds(up)) {
                        shorten_y = !BLOCKS[self->data[chunk_pos_to_index(up)]].is_liquid();
                    } else {
                        shorten_y = !BLOCKS[world_get_data(self->world, glms_ivec3_add(up, self->position))].is_liquid();
                    }
                }

                for (enum Direction d = 0; d < 6; d++) {
                    ivec3s dv = DIR2IVEC3S(d);
                    ivec3s neighbor = glms_ivec3_add(pos, dv), wneighbor = glms_ivec3_add(wpos, dv);

                    if (chunk_in_bounds(neighbor)) {
                        neighbor_block = BLOCKS[self->data[chunk_pos_to_index(neighbor)]];
                    } else {
                        neighbor_block = BLOCKS[world_get_data(self->world, wneighbor)];
                    }

                    bool neighbor_transparent = neighbor_block.is_transparent(self->world, wneighbor);
                    if (neighbor_transparent && (
                        (pass == FULL && !transparent) ||
                        (transparent && neighbor_block.id != block.id))) {

                        emit_face(
                            transparent ? &self->meshes.transparent : &self->meshes.base, fpos, d,
                            atlas_offset(state.block_atlas.atlas, block.get_texture_location(self->world, wpos, d)),
                            state.block_atlas.atlas.sprite_unit, transparent, shorten_y);
                    }
                }
            }
        }
    }

    if (pass == FULL) {
        mesh_finalize(&self->meshes.base, false);
    }

    mesh_finalize(&self->meshes.transparent, true);
}

void chunk_render(struct Chunk *self) {
    if ((self->dirty || self->depth_sort) &&
        self->world->throttles.mesh.count < self->world->throttles.mesh.max) {
        mesh(self, self->dirty ? FULL : TRANSPARENCY);

        self->dirty = false;
        self->depth_sort = false;
        self->world->throttles.mesh.count++;
    }

    mesh_render(&self->meshes.base);
}

void chunk_render_transparent(struct Chunk *self) {
    mesh_render(&self->meshes.transparent);
}

void chunk_update(struct Chunk *self) {
    // Depth sort the transparent mesh if
    // (1) the player is inside of this chunk and their block position changed
    // (2) the player has moved chunks AND this chunk is close (within 1 chunk distance)
    struct EntityPlayer *player = &self->world->player;
    self->depth_sort =
        (!ivec3scmp(self->offset, player->offset) && player->block_pos_changed) ||
        (player->offset_changed && glms_ivec3_norm(glms_ivec3_sub(self->offset, player->offset)) < 2);
}

void chunk_tick(struct Chunk *self) {

}