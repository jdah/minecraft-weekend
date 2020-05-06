// #include "player.h"
// #include "../state.h"

// void player_init(struct EntityPlayer *self, struct World *world) {
//     memset(self, 0, sizeof(struct EntityPlayer));
//     self->world = world;
//     perspective_camera_init(&self->camera, radians(75.0f));
// }

// void player_destroy(struct EntityPlayer *self) {

// }

// void player_render(struct EntityPlayer *self) {
//     state.renderer.perspective_camera = self->camera;
// }

// void player_update(struct EntityPlayer *self) {
//     const f32 mouse_sensitivity = 3.2f;
//     perspective_camera_update(&self->camera);

//     for (size_t i = 0; i < 10; i++) {
//         if (state.window->keyboard.keys[GLFW_KEY_0 + i].down) {
//             self->selected_block = ((enum BlockId[]) {
//                 TORCH,
//                 STONE,
//                 PLANKS,
//                 SAND,
//                 GLASS,
//                 WATER,
//                 LOG,
//                 LEAVES,
//                 ROSE,
//                 COAL
//             })[i];
//         }
//     }
// }

// bool raycast_block_fn(ivec3s v) {
//     return world_get_block(&state.world, v) != 0;
// }

// void player_tick(struct EntityPlayer *self) {
    
    

//     // update look at block
//     const f32 reach = 6.0f;
//     self->has_look_block = ray_block(
//         (struct Ray) { .origin = self->camera.position, .direction = self->camera.direction },
//         reach, raycast_block_fn, &self->look_block, &self->look_face);

//     // place/destroy blocks
//     if (self->has_look_block) {
//         if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed_tick) {
//             world_set_block(self->world, self->look_block, 0);
//         }

//         if (state.window->mouse.buttons[GLFW_MOUSE_BUTTON_RIGHT].pressed_tick) {
//             world_set_block(
//                 self->world,
//                 glms_ivec3_add(self->look_block, DIR2IVEC3S(self->look_face)),
//                 self->selected_block);
//         }
//     }
// }