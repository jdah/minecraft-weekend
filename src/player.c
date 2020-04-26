#include "include/player.h"
#include "include/state.h"

void player_init(struct EntityPlayer *self, struct World *world) {
    memset(self, 0, sizeof(struct EntityPlayer));
    self->world = world;
    self->camera = camera_create(radians(75.0f));
}

void player_destroy(struct EntityPlayer *self) {

}

void player_render(struct EntityPlayer *self) {

}

void player_update(struct EntityPlayer *self) {
    const f32 mouse_sensitivity = 3.2f;
    camera_update(&self->camera);
    self->camera.pitch -= state.window->mouse.delta.y / (((f32) state.window->frame_delta) / (mouse_sensitivity * 10000.0f));
    self->camera.yaw -= state.window->mouse.delta.x / (((f32) state.window->frame_delta) / (mouse_sensitivity * 10000.0f));

    ivec3s block_pos = world_pos_to_block(self->camera.position);
    ivec3s offset = world_pos_to_offset(block_pos);

    if (ivec3scmp(block_pos, self->block_pos)) {
        self->block_pos = block_pos;
        self->block_pos_changed = true;
    } else {
        self->block_pos_changed = false;
    }

    if (ivec3scmp(offset, self->offset)) {
        self->offset = offset;
        self->offset_changed = true;
    } else {
        self->offset_changed = false;
    }

    for (size_t i = 0; i < 10; i++) {
        if (state.window->keyboard.keys[GLFW_KEY_0 + i].down) {
            self->selected_block = ((enum BlockId[]) {
                DIRT,
                STONE,
                PLANKS,
                SAND,
                GLASS,
                WATER,
                LOG,
                LEAVES,
                ROSE,
                COAL
            })[i];
        }
    }
}

bool raycast_block_fn(ivec3s v) {
    return world_get_data(&state.world, v) != 0;
}

void player_tick(struct EntityPlayer *self) {
    const f32 speed = 0.22f;
    vec3s movement, direction, forward, right;
    movement = GLMS_VEC3_ZERO;
    direction = GLMS_VEC3_ZERO;
    forward = (vec3s) {{ sinf(self->camera.yaw), 0, cosf(self->camera.yaw) }};
    right = glms_vec3_cross((vec3s) {{ 0.0f, 1.0f, 0.0f }}, forward);

    if (state.window->keyboard.keys[GLFW_KEY_W].down) {
        direction = glms_vec3_add(direction, forward);
    }

    if (state.window->keyboard.keys[GLFW_KEY_S].down) {
        direction = glms_vec3_sub(direction, forward);
    }

    if (state.window->keyboard.keys[GLFW_KEY_A].down) {
        direction = glms_vec3_add(direction, right);
    }

    if (state.window->keyboard.keys[GLFW_KEY_D].down) {
        direction = glms_vec3_sub(direction, right);
    }

    if (state.window->keyboard.keys[GLFW_KEY_SPACE].down) {
        direction = glms_vec3_add(direction, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    }

    if (state.window->keyboard.keys[GLFW_KEY_LEFT_SHIFT].down) {
        direction = glms_vec3_sub(direction, (vec3s) {{ 0.0f, 1.0f, 0.0f }});
    }

    if (isnan(glms_vec3_norm(direction))) {
        movement = GLMS_VEC3_ZERO;
    } else {
        movement = direction;
        movement = glms_vec3_normalize(movement);
        movement = glms_vec3_scale(movement, speed);
    }

    self->camera.position = glms_vec3_add(self->camera.position, movement);

    // update look at block
    const f32 reach = 6.0f;
    self->has_look_block = ray_block(
        (struct Ray) { .origin = self->camera.position, .direction = self->camera.direction },
        reach, raycast_block_fn, &self->look_block, &self->look_face);

    // place/destroy blocks
    if (self->has_look_block) {
        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed_tick) {
            world_set_data(self->world, self->look_block, 0);
        }

        if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_RIGHT].pressed_tick) {
            world_set_data(
                self->world,
                glms_ivec3_add(self->look_block, DIR2IVEC3S(self->look_face)),
                self->selected_block);
        }
    }
}