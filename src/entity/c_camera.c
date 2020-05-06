#include "ecs.h"
#include "c_camera.h"
#include "c_position.h"

static void init(struct CameraComponent *c_camera, struct Entity entity) {
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);
    perspective_camera_init(&c_camera->camera, radians(75.0f));
    c_camera->camera.position = c_position->position;
}

static void update(struct CameraComponent *c_camera, struct Entity entity) {
    struct PositionComponent *c_position = ecs_get(entity, C_POSITION);
    c_camera->camera.position = c_position->position;
    perspective_camera_update(&c_camera->camera);
}

void c_camera_init(struct ECS *ecs) {
    ecs_register(C_CAMERA, struct CameraComponent, ecs, ((union ECSSystem) {
        .init = (ECSSubscriber) init,
        .destroy = NULL,
        .render = NULL,
        .update = (ECSSubscriber) update,
        .tick = NULL
    }));
}