#include "blockmesh.h"
#include "chunkmesh.h"
#include "../block/block.h"
#include "chunk.h"

static const u32 FACE_INDICES[] = {1, 0, 3, 1, 3, 2};
static const u32 UNIQUE_INDICES[] = {1, 0, 5, 2};
static const u32 CUBE_INDICES[] = {
    1, 0, 3, 1, 3, 2, // north (-z)
    4, 5, 6, 4, 6, 7, // south (+z)
    5, 1, 2, 5, 2, 6, // east (+x)
    0, 4, 7, 0, 7, 3, // west (-x)
    2, 3, 7, 2, 7, 6, // top (+y)
    5, 4, 0, 5, 0, 1, // bottom (-y)
};

static const f32 FACE_CENTERS[] = {
    0.5f, 0.5f, 0.0f, // north (-z)
    0.5f, 0.5f, 1.0f, // south (+z)
    1.0f, 0.5f, 0.5f, // east (+x)
    0.0f, 0.5f, 0.5f, // west (-x)
    0.5f, 1.0f, 0.5f, // top (+y)
    0.5f, 0.0f, 0.5f, // bottom (-y)
};

static const u32 SPRITE_INDICES[] = {
    3, 5, 0, 3, 6, 5,
    2, 1, 4, 2, 4, 7,

    3, 0, 5, 3, 5, 6,
    2, 4, 1, 2, 7, 4
};

static const f32 CUBE_VERTICES[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,

    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
};

static const f32 CUBE_UVS[] = {
    1, 0,
    0, 0,
    0, 1,
    1, 1
};


// appends data to chunk buffer
#define APPEND(_type, _buffer, _v) ({\
        ((_type *) (_buffer)->data)[(_buffer)->elements++] = (_v);\
        (_buffer)->index += sizeof(_type);\
    })

// formats light data for usage with chunk shader, see chunk.vs for usage
#define MAKE_LIGHT_DATA(_direction, _light) ((_light) | ((_direction) << 20))

// emits an entire sprite to the specified chunk mesh
void blockmesh_sprite(struct ChunkMesh *mesh, struct BlockMeshParams params) {
    struct ChunkMeshBuffer
        *data = &mesh->buffers[DATA],
        *faces = &mesh->buffers[FACES],
        *indices = &mesh->buffers[INDICES];

    // emitting 4 transparent faces
    for (size_t i = 0; i < 4; i++) {
        struct Face face = {
            .indices_base = indices->elements + (i * 6),
            .position = params.position
        };

        memcpy(((struct Face *) faces->data) + faces->elements, &face, sizeof(struct Face));
        faces->index += sizeof(struct Face);
        faces->elements++;
    }

    // emit vertices
    for (size_t i = 0; i < 8;  i++) {
        const f32 *vertex = &CUBE_VERTICES[i * 3];
        APPEND(f32, data, params.position.x + vertex[0]);
        APPEND(f32, data, params.position.y + vertex[1]);
        APPEND(f32, data, params.position.z + vertex[2]);
        APPEND(f32, data, CUBE_UVS[((i % 4) * 2) + 0] ? params.uv_max.x : params.uv_min.x);
        APPEND(f32, data, CUBE_UVS[((i % 4) * 2) + 1] ? params.uv_max.y : params.uv_min.y);

        // use lighting data of the current block, sprites are transparent so
        // they should have lighting values
        // set direction to UP so lighting is full
        APPEND(u32, data, MAKE_LIGHT_DATA(UP, chunk_data_to_light(params.data)));
    }

    // emit indices
    for (size_t i = 0; i < 24; i++) {
        APPEND(u32, indices, mesh->vertex_count + SPRITE_INDICES[i]);
    }

    // emitted 8 vertices
    mesh->vertex_count += 8; 
}

// emits a single block face to the specified block mesh
void blockmesh_face(struct ChunkMesh *mesh, struct BlockMeshParams params) {
    struct ChunkMeshBuffer
        *data = &mesh->buffers[DATA],
        *faces = &mesh->buffers[FACES],
        *indices = &mesh->buffers[INDICES];
    
    // add this face into the face buffer if it's transparent
    if (params.block->transparent) {
        struct Face face = {
            .indices_base = indices->elements,
            .position = glms_vec3_add((vec3s) {{
                FACE_CENTERS[(params.direction * 3) + 0],
                FACE_CENTERS[(params.direction * 3) + 1],
                FACE_CENTERS[(params.direction * 3) + 2]
            }}, params.position)
        };

        memcpy(((struct Face *) faces->data) + faces->elements, &face, sizeof(struct Face));
        faces->index += sizeof(struct Face);
        faces->elements++;
    }

    // emit vertices
    for (size_t i = 0; i < 4;  i++) {
        const f32 *vertex = &CUBE_VERTICES[CUBE_INDICES[(params.direction * 6) + UNIQUE_INDICES[i]] * 3];
        APPEND(f32, data, params.position.x + params.offset.x + (vertex[0] * params.size.x));
        APPEND(f32, data, params.position.y + params.offset.y + (vertex[1] * params.size.y));
        APPEND(f32, data, params.position.z + params.offset.z + (vertex[2] * params.size.z));
        APPEND(f32, data, CUBE_UVS[(i * 2) + 0] ? params.uv_max.x : params.uv_min.x);
        APPEND(f32, data, CUBE_UVS[(i * 2) + 1] ? params.uv_max.y : params.uv_min.y);

        // use lighting data of this face's NEIGHBOR to light it if the block
        // is not transparent. if it is transparent, use the current block's lighting
        APPEND(u32, data,
            MAKE_LIGHT_DATA(
                params.direction,
                chunk_data_to_light(params.block->transparent ? params.data : params.data_n)));
    }

    // emit indices
    for (size_t i = 0; i < 6; i++) {
        APPEND(u32, indices, mesh->vertex_count + FACE_INDICES[i]);
    }

    // emitted 4 more vertices, bump the vertex count
    mesh->vertex_count += 4;
}