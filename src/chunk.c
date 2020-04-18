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
} buffers[7];

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

void buffer_prepare(struct MeshBuffer *self, f32 *data, u16 *indices) {
    memset(self, 0, sizeof(struct MeshBuffer));
    self->data_buffer = data;
    self->index_buffer = indices;
}

void mesh_prepare(struct Mesh *self) {
    for (size_t i = 0; i < 7; i++) {
        buffer_prepare(&self->buffers[i], buffers[i].data, buffers[i].indices);
    }
}

void mesh_finalize(struct Mesh *self) {
    // Reverse the order of indices in the negative-facing transparency direcetions.
    // Since traversal/render order is positive we need these to appear front to back
    // in their own respective direction.
    for (enum Direction d = 0; d < 6; d++) {
        struct MeshBuffer *buffer = &self->buffers[1 + d];
        if (d == NORTH || d == WEST || d == UP) {
            const size_t sz = 6 * sizeof(u16);
            void *a = buffer->index_buffer;
            size_t len = (buffer->indices_index / 6);
            
            if (len == 0) {
                continue;
            }

            size_t i = len - 1, j = 0;

            while (i > j) {
                u16 t[6];

                // swap a[i] and a[j]
                memcpy(t, a + (i * sz), sz);
                memcpy(a + (i * sz), a + (j * sz), sz);
                memcpy(a + (j * sz), t, sz);
                
                i--;
                j++;
            }

            // printf("%zu\n", buffer->indices_index * 2);
            // memset(buffer->index_buffer, 0, buffer->indices_index * sizeof(u16));
            // buffer->index_buffer[0] = 4586;
            // buffer->indices_index = 6;
            // memset(buffer->data_buffer, 0, buffer->data_index * sizeof(f32));
        }
    }

    size_t data_len = 0, indices_len = 0;

    for (size_t i = 0; i < 7; i++) {
        data_len += self->buffers[i].data_index;
        indices_len += self->buffers[i].indices_index;
    }

    // Move data into two contiguous buffers
    f32 *data = (f32*) malloc((data_len * sizeof(f32)) + (indices_len * sizeof(u16)));
    u16 *indices = (u16*) (data + data_len);

    size_t di = 0, ii = 0, vc = 0;
    for (size_t i = 0; i < 7; i++) {
        memcpy(data + di, self->buffers[i].data_buffer, self->buffers[i].data_index * sizeof(f32));
        memcpy(indices + ii, self->buffers[i].index_buffer, self->buffers[i].indices_index * sizeof(u16));

        self->buffers[i].indices_offset = ii;
        self->buffers[i].indices_base = vc;
        self->buffers[i].indices_count = self->buffers[i].indices_index;

        vc += self->buffers[i].vertex_count;
        di += self->buffers[i].data_index;
        ii += self->buffers[i].indices_index;
    }

    vbo_buffer(self->vbo, data, 0, di * sizeof(f32));
    vbo_buffer(self->ibo, indices, 0, ii * sizeof(u16));

    free(data);
}
 
void mesh_render(struct Mesh *self) {
    shader_bind(state.shader);
    shader_uniform_camera(state.shader, state.world.player.camera);
    shader_uniform_mat4(state.shader, "m", glms_translate(glms_mat4_identity(), IVEC3S2V(self->chunk->position)));
    shader_uniform_texture2D(state.shader, "tex", state.atlas.texture, 0);

    const size_t vertex_size = 8 * sizeof(f32);
    vao_attr(self->vao, self->vbo, 0, 3, GL_FLOAT, vertex_size, 0 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 1, 2, GL_FLOAT, vertex_size, 3 * sizeof(f32));
    vao_attr(self->vao, self->vbo, 2, 3, GL_FLOAT, vertex_size, 5 * sizeof(f32));

    vao_bind(self->vao);
    vbo_bind(self->ibo);

    for (size_t i = 0; i < 7; i++) {
        struct MeshBuffer buffer = self->buffers[i];
        if (buffer.indices_count > 0) {
            if (i > 0) glDepthMask(GL_FALSE);
            glDrawElementsBaseVertex(
                GL_TRIANGLES, buffer.indices_count,
                GL_UNSIGNED_SHORT, buffer.indices_offset * sizeof(u16), buffer.indices_base);
            if (i > 0) glDepthMask(GL_TRUE);
        }
    }
}

static void emit_face(
    struct MeshBuffer *buffer, vec3s position, enum Direction direction, 
    vec2s uv_offset, vec2s uv_unit, bool transparent) {
    // Emit vertices
    for (size_t i = 0; i < 4;  i++) {
        const f32 *vertex = &CUBE_VERTICES[CUBE_INDICES[(direction * 6) + UNIQUE_INDICES[i]] * 3];
        buffer->data_buffer[buffer->data_index++] = position.x + vertex[0];
        buffer->data_buffer[buffer->data_index++] = position.y + vertex[1];
        buffer->data_buffer[buffer->data_index++] = position.z + vertex[2];
        buffer->data_buffer[buffer->data_index++] = uv_offset.x + (uv_unit.x * CUBE_UVS[(i * 2) + 0]);
        buffer->data_buffer[buffer->data_index++] = uv_offset.y + (uv_unit.y * CUBE_UVS[(i * 2) + 1]);

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
        }

        for (size_t j = 0; j < 3; j++) {
            buffer->data_buffer[buffer->data_index++] = color;
        } 
    }

    // Emit indices
    for (size_t i = 0; i < 6; i++) {
        buffer->index_buffer[buffer->indices_index++] = buffer->vertex_count + FACE_INDICES[i];
    }

    // Emitted 4 more vertices, bump the vertex count
    buffer->vertex_count += 4;
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
    mesh_prepare(&self->mesh);

    chunk_foreach(pos) {
        vec3s fpos = IVEC3S2V(pos);
        ivec3s wpos = glms_ivec3_add(pos, self->position);

        u32 data = self->data[chunk_pos_to_index(pos)];
        if(data != 0) {
            struct Block block = BLOCKS[data], neighbor_block;
            bool transparent = block.is_transparent(self->world, wpos);

            for (enum Direction d = 0; d < 6; d++) {
                ivec3s dv = DIR2IVEC3S(d);
                ivec3s neighbor = glms_ivec3_add(pos, dv), wneighbor = glms_ivec3_add(wpos, dv);

                if (chunk_in_bounds(neighbor)) {
                    neighbor_block = BLOCKS[self->data[chunk_pos_to_index(neighbor)]];
                } else {
                    neighbor_block = BLOCKS[world_get_data(self->world, wneighbor)];
                }

                bool neighbor_transparent = neighbor_block.is_transparent(self->world, wneighbor);
                if (neighbor_transparent && (!transparent ||
                    (transparent && neighbor_block.id != block.id))) {
                    emit_face(
                        &self->mesh.buffers[transparent ? (1 + d) : 0], fpos, d,
                        atlas_offset(state.atlas, BLOCKS[data].get_texture_location(self->world, wpos, d)),
                        state.atlas.sprite_unit, false);
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