#ifndef BLOCKMESH_H
#define BLOCKMESH_H

#include "../util/util.h"

// forward declarations
struct Block;
struct ChunkMesh;

enum BlockMeshType {
    BLOCKMESH_DEFAULT,
    BLOCKMESH_SPRITE,
    BLOCKMESH_LIQUID,
    BLOCKMESH_CUSTOM
};

struct BlockMeshParams {
    struct Block *block, *block_n;
    u64 data, data_n;
    vec2s uv_min, uv_max;
    vec3s position, offset, size;
    enum Direction direction;
};

void blockmesh_sprite(struct ChunkMesh *mesh, struct BlockMeshParams params);
void blockmesh_torch(struct ChunkMesh *mesh, struct BlockMeshParams params);
void blockmesh_face(struct ChunkMesh *mesh, struct BlockMeshParams params);

#endif