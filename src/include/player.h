#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include "camera.h"
#include "block.h"

// Forward declaration
struct World;

struct EntityPlayer {
    struct World *world;
    struct Camera camera;

    bool has_look_block;
    ivec3s look_block;
    enum Direction look_face;

    // Player's current chunk offset, block position
    ivec3s offset;
    ivec3s block_pos;

    // true if offset or block position changed since the last update()
    bool offset_changed, block_pos_changed;

    enum BlockId selected_block;
};

void player_init(struct EntityPlayer *self, struct World *world);
void player_destroy(struct EntityPlayer *self);
void player_render(struct EntityPlayer *self);
void player_update(struct EntityPlayer *self);
void player_tick(struct EntityPlayer *self);

#endif