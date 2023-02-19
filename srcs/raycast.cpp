#include "raycast.h"

//http://www.cse.yorku.ca/~amana/research/grid.pdf

static inline void initDDA(glm::ivec3 &step, glm::vec3 &max, glm::vec3 &delta, glm::vec3 &origin, glm::vec3 &direction) {
    step.x = (direction.x < 0) ? -1 : 1;
    step.y = (direction.y < 0) ? -1 : 1;
    step.z = (direction.z < 0) ? -1 : 1;
    delta = 1.0f / abs(direction);
    if (step.x > 0) {
        max.x = ceil(origin.x) - origin.x;
        max.x += (max.x == 0);
    }
    else
        max.x = origin.x - floor(origin.x);
    if (step.y > 0) {
        max.y = ceil(origin.y) - origin.y;
        max.y += (max.y == 0);
    }
    else
        max.y = origin.y - floor(origin.y);
    if (step.z > 0) {
        max.z = ceil(origin.z) - origin.z;
        max.z += (max.z == 0);
    }
    else
        max.z = origin.z - floor(origin.z);
    max *= delta;
}

RayCastInfo rayCastGetCube(glm::vec3 origin, glm::vec3 direction, float range) { //need to modify it has some problem in certain coord and inf or nan might be a problem
    Chunk* chunk;
    glm::ivec3 step;
    glm::ivec3 pos;
    glm::vec3 delta;
    glm::vec3 max;
    RayCastInfo result;
    float distance;

    distance = 0;
    direction = glm::normalize(direction);
    pos = floor(origin);
    if (!(chunk = GetChunk(pos.x >> 4, pos.z >> 4)))
        return (result);
    pos.x &= 0xF;
    pos.z &= 0xF;
    if (chunk->GetCube(pos.x, pos.y, pos.z) != AIR) {
        result.id = chunk->GetCube(pos.x, pos.y, pos.z);
        result.position = glm::ivec3(floor(origin));
        return (result);
    }
    initDDA(step, max, delta, origin, direction);
    while (fabs(distance) < range) {
        result.side = pos;
        if (max.x < max.y) {
            if (max.x < max.z) {
                distance = max.x;
                max.x += delta.x;
                pos.x += step.x;
            }
            else {
                distance = max.z;
                max.z += delta.z;
                pos.z += step.z;
            }
        }
        else {
            if (max.y < max.z) {
                distance = max.y;
                max.y += delta.y;
                pos.y += step.y;
            }
            else {
                distance = max.z;
                max.z += delta.z;
                pos.z += step.z;
            }
        }
        if (chunk->GetCube(pos.x, pos.y, pos.z) != AIR) {
            result.id = chunk->GetCube(pos.x, pos.y, pos.z);
            result.position = glm::ivec3(pos.x + (chunk->posx << 4), pos.y, pos.z + (chunk->posz << 4));
            result.side.x += (chunk->posx << 4);
            result.side.z += (chunk->posz << 4);
            result.range = fabs(distance);
            return (result);
        }
    }
    return (result);
}