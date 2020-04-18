#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include "camera.h"

// Forward declaration
struct World;

struct EntityPlayer {
    struct World *world;
    struct Camera camera;

    bool has_look_block;
    ivec3s look_block;
    enum Direction look_face;
};

void player_init(struct EntityPlayer *self, struct World *world);
void player_destroy(struct EntityPlayer *self);
void player_render(struct EntityPlayer *self);
void player_update(struct EntityPlayer *self);
void player_tick(struct EntityPlayer *self);

#endif