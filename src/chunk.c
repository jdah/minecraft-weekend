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

// global mesh data buffers
// TODO: if threading is introduced, make these per-thread
struct {
    f32 data[(16 * 256 * 16) * 8 * 5 * sizeof(f32)];
    u16 indices[(16 * 256 * 16) * 36 * sizeof(u16)];
} buffers;

void mesh_init(struct Mesh *self, struct Chunk *chunk) {
    memset(self, 0, sizeof(struct Mesh));
    self->chunk = chunk;
    self->vao = vao_create();
    self->vbo = vbo_create(GL_ARRAY_BUFFER, false);
    self->ibo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, false);   
}

void mesh_destroy(struct Mesh *self) {
    vao_destroy(self->vao);
    vbo_destroy(self->vbo);
    vbo_destroy(self->ibo);
}

void mesh_prepare(struct Mesh *self, f32 *data, u16 *indices) {
    self->vertex_count = 0;
    self->data_index = 0;
    self->indices_index = 0;
    self->data_buffer = data;
    self->index_buffer = indices;
    self->index_count = 0;
}

void mesh_finalize(struct Mesh *self) {
    self->index_count = self->indices_index;
    vbo_buffer(self->vbo, self->data_buffer, 0, self->data_index * sizeof(f32));
    vbo_buffer(self->ibo, self->index_buffer, 0, self->indices_index * sizeof(u16));
}
 
void mesh_render(struct Mesh *self) {
    shader_bind(state.shader);
    shader_uniform_camera(state.shader, state.world.player.camera);
    shader_uniform_mat4(state.shader, "m", glms_translate(glms_mat4_identity(), IVEC3S2V(self->chunk->position)));
    shader_uniform_texture2D(state.shader, "tex", state.atlas.texture, 0);

    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, 8 * sizeof(f32), 0 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, 8 * sizeof(f32), 3 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 2, 3, GL_FLOAT, 8 * sizeof(f32), 5 * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);
    glDrawElements(GL_TRIANGLES, self->index_count, GL_UNSIGNED_SHORT, 0);
}

static void emit_face(
    struct Mesh *mesh, vec3s position, enum Direction direction, 
    vec2s uv_offset, vec2s uv_unit) {
    // Emit vertices
    for (size_t i = 0; i < 4;  i++) {
        const f32 *vertex = &CUBE_VERTICES[CUBE_INDICES[(direction * 6) + UNIQUE_INDICES[i]] * 3];
        mesh->data_buffer[mesh->data_index++] = position.x + vertex[0];
        mesh->data_buffer[mesh->data_index++] = position.y + vertex[1];
        mesh->data_buffer[mesh->data_index++] = position.z + vertex[2];
        mesh->data_buffer[mesh->data_index++] = uv_offset.x + (uv_unit.x * CUBE_UVS[(i * 2) + 0]);
        mesh->data_buffer[mesh->data_index++] = uv_offset.y + (uv_unit.y * CUBE_UVS[(i * 2) + 1]);

        // TODO: lighting
        // Vary color according to face direction
        f32 color;
        switch (direction) {
            case UP:
                color = 1.0f;
                break;
            case NORTH:
            case SOUTH:
                color = 0.92f;
                break;
            case EAST:
            case WEST:
                color = 0.86f;
                break;
            case DOWN:
                color = 0.78f;
                break;
        }

        for (size_t j = 0; j < 3; j++) {
            mesh->data_buffer[mesh->data_index++] = color;
        } 
    }

    // Emit indices
    for (size_t i = 0; i < 6; i++) {
        mesh->index_buffer[mesh->indices_index++] = mesh->vertex_count + FACE_INDICES[i];
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
    mesh_init(&self->mesh, self);
}

void chunk_destroy(struct Chunk *self) {
    free(self->data);
    mesh_destroy(&self->mesh);
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

static void mesh(struct Chunk *self) {
    mesh_prepare(&self->mesh, buffers.data, buffers.indices);

    chunk_foreach(pos) {
        vec3s fpos = IVEC3S2V(pos);
        ivec3s wpos = glms_ivec3_add(pos, self->position);
        u32 data = self->data[chunk_pos_to_index(pos)];

        if(data != 0) {            
            for (enum Direction d = 0; d < 6; d++) {
                ivec3s dv = DIR2IVEC3S(d);
                ivec3s neighbor = glms_ivec3_add(pos, dv), wneighbor = glms_ivec3_add(wpos, dv);

                bool visible = false;

                if (chunk_in_bounds(neighbor)) {
                    visible = BLOCKS[self->data[chunk_pos_to_index(neighbor)]].is_transparent(self->world, wneighbor);
                } else {
                    visible = world_in_bounds(self->world, wneighbor) &&
                        BLOCKS[world_get_data(self->world, wneighbor)].is_transparent(self->world, wneighbor);
                }

                if (visible) {
                    emit_face(
                        &self->mesh, fpos, d,
                        atlas_offset(state.atlas, BLOCKS[data].get_texture_location(self->world, wpos, d)),
                        state.atlas.sprite_unit);
                }
            }
        }
    }

    mesh_finalize(&self->mesh);
}

void chunk_render(struct Chunk *self) {
    if (self->dirty) {
        mesh(self);
        self->dirty = false;
    }

    mesh_render(&self->mesh);
}

void chunk_update(struct Chunk *self) {

}

void chunk_tick(struct Chunk *self) {

}