#include "chunk.h"
#include "camera.h"

#define PLAYER_RANGE 5

glm::ivec3 getPointedCube(Camera &camera) {     //test the function
    Chunk* chunk;
    glm::vec3 origin = camera.posision - 0.5f;

    for (int n = 0; n < PLAYER_RANGE; n++) {
        chunk = GetChunk((int)origin.x >> 4, (int)origin.z >> 4);   // change later to prevent getting the chunk on every block
        if (chunk && chunk->GetCube((int)origin.x & 0xF, (int)origin.y & 0xFF, (int)origin.z & 0xF) != AIR)
            return (glm::ivec3(origin));
        origin += camera.direction;
    }
    return (glm::ivec3(-1));
}

static void removePointedCube(Camera& camera) {
    Chunk* chunk;
    glm::ivec3 cubeCood = getPointedCube(camera);
    if (cubeCood.y < 0)
        return;
    chunk = GetChunk(cubeCood.x >> 4, cubeCood.z >> 4);
    chunk->SetCube(AIR, cubeCood.x & 0xF, cubeCood.y & 0xFF, cubeCood.z & 0xF);
    chunk->UpdateCube(cubeCood.x & 0xF, cubeCood.z & 0xF);
}

void getMouseEvent(GLFWwindow* window, Camera &camera) {
    int mouseState;

    mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouseState == GLFW_PRESS)
        removePointedCube(camera);
}