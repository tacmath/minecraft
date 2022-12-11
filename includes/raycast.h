#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp>
#include "chunk.h"

struct RayCastInfo {
    unsigned char id;
    glm::ivec3 position;
    glm::ivec3 side;
    float		range;

    RayCastInfo() {
        id = 0;
        position = glm::ivec3(0);
        range = 0;
    }
};

RayCastInfo rayCastGetCube(glm::vec3 origin, glm::vec3 direction, int range);

#endif