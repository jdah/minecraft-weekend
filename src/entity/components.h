#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "c_position.h"
#include "c_camera.h"
#include "c_control.h"
#include "c_physics.h"

extern void c_position_init();
extern void c_camera_init();
extern void c_control_init();
extern void c_physics_init();

#define _ecs_init_internal(ecs)\
    c_position_init(ecs);\
    c_camera_init(ecs);\
    c_control_init(ecs);\
    c_physics_init(ecs);

#define ECSCOMPONENT_LAST C_PHYSICS
enum ECSComponent {
    C_POSITION = 0,
    C_CAMERA,
    C_CONTROL,
    C_PHYSICS
};

#endif