#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "c_position.h"
#include "c_camera.h"
#include "c_control.h"
#include "c_physics.h"
#include "c_movement.h"
#include "c_blocklook.h"
#include "c_debug.h"

extern void c_position_init();
extern void c_camera_init();
extern void c_control_init();
extern void c_physics_init();
extern void c_movement_init();
extern void c_blocklook_init();
extern void c_debug_init();

#define _ecs_init_internal(ecs)\
    c_position_init(ecs);\
    c_camera_init(ecs);\
    c_control_init(ecs);\
    c_physics_init(ecs);\
    c_movement_init(ecs);\
    c_blocklook_init(ecs);\
    c_debug_init(ecs);

#define ECSCOMPONENT_LAST C_DEBUG
enum ECSComponent {
    C_POSITION = 0,
    C_CAMERA,
    C_CONTROL,
    C_PHYSICS,
    C_MOVEMENT,
    C_BLOCKLOOK,
    C_DEBUG
};

#endif