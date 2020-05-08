#ifndef C_LIGHT_H
#define C_LIGHT_H

#include "../util/util.h"
#include "../world/light.h"

struct LightComponent {
    Torchlight light;

    struct {
        ivec3s pos;
        Torchlight light;
        bool enabled;
    } last;
    
    struct {
        bool enabled: 1;
    } flags;
};

#endif