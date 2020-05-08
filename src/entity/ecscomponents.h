#ifndef ECSCOMPONENTS_H
#define ECSCOMPONENTS_H

#include "c_position.h"
#include "c_camera.h"
#include "c_control.h"
#include "c_physics.h"
#include "c_movement.h"
#include "c_blocklook.h"
#include "c_debug.h"
#include "c_light.h"

#define _ECS_DECL_COMPONENT(_name)\
    extern void _name##_init();\
    _name##_init(ecs);

static inline void _ecs_init_internal(struct ECS *ecs) {
    _ECS_DECL_COMPONENT(c_position);
    _ECS_DECL_COMPONENT(c_camera);
    _ECS_DECL_COMPONENT(c_control);
    _ECS_DECL_COMPONENT(c_physics);
    _ECS_DECL_COMPONENT(c_movement);
    _ECS_DECL_COMPONENT(c_blocklook);
    _ECS_DECL_COMPONENT(c_debug);
    _ECS_DECL_COMPONENT(c_light);
}

#define ECSCOMPONENT_LAST C_LIGHT
enum ECSComponent {
    C_POSITION = 0,
    C_CAMERA,
    C_CONTROL,
    C_PHYSICS,
    C_MOVEMENT,
    C_BLOCKLOOK,
    C_DEBUG,
    C_LIGHT
};

#endif