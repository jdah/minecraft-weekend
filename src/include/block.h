#ifndef BLOCK_H
#define BLOCK_H

#include "util.h"
#include "blockatlas.h"
#include "blockmesh.h"
#include "light.h"

// Forward declaration
struct World;

enum BlockId {
    AIR = 0,
    GRASS = 1,
    DIRT = 2,
    STONE = 3,
    SAND = 4,
    WATER = 5,
    GLASS = 6,
    LOG = 7,
    LEAVES = 8,
    ROSE = 9,
    BUTTERCUP = 10,
    COAL = 11,
    COPPER = 12,
    LAVA = 13,
    CLAY = 14,
    GRAVEL = 15,
    PLANKS = 16,
    TORCH = 17
};

// Always the highest valid block id
#define BLOCK_ID_LAST TORCH

struct Block {
    enum BlockId id;

    // if true, this block is treated as it is transparent
    bool transparent;

    // if true, this block is treated like a liquid
    bool liquid;

    // if true, this block is treated as a light source
    bool can_emit_light;

    // if true, this block's textures are determined with get_animation_frames
    bool animated;

    // the mesh type of this block, see blockmesh.h
    enum BlockMeshType mesh_type;

    ivec2s (*get_texture_location)(struct World *world, ivec3s pos, enum Direction d);
    void (*get_animation_frames)(ivec2s out[BLOCK_ATLAS_FRAMES]);
    Torchlight (*get_torchlight)(struct World *world, ivec3s pos);
};

#define MAX_BLOCK_ID INT16_MAX

struct Block BLOCK_DEFAULT;
struct Block BLOCKS[MAX_BLOCK_ID];

void block_init();

#endif