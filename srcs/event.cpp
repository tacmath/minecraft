#include "chunk.h"
#include "camera.h"
#include "event.h"
#include "debug.h"
#include "raycast.h"

void Event::removePointedCube() {
    Chunk* chunk;
    glm::ivec3 pos;

    if (player->selectedCube.id == AIR)
        return;
    pos = player->selectedCube.position;
    chunk = GetChunk(pos.x >> 4, pos.z >> 4);
    chunk->SetCube(AIR, pos.x & 0xF, pos.y & 0xFF, pos.z & 0xF);
    chunk->UpdateCube(pos.x & 0xF, pos.z & 0xF);
    player->selectedCube = rayCastGetCube(player->position, player->look, PLAYER_RANGE);
}

void Event::placeCube() {
    Chunk* chunk;
    glm::ivec3 pos;

    if (player->selectedCube.id == AIR || player->selectedCube.range < 1.5)
        return;
    pos = player->selectedCube.side;
    chunk = GetChunk(pos.x >> 4, pos.z >> 4);
    chunk->SetCube(3, pos.x & 0xF, pos.y & 0xFF, pos.z & 0xF);
    chunk->UpdateCube(pos.x & 0xF, pos.z & 0xF);
    player->selectedCube = rayCastGetCube(player->position, player->look, PLAYER_RANGE);
}

void keyToogleCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static ToggleData* toggleData = (ToggleData*)glfwGetWindowUserPointer(window);
    static bool hasNormalShader = true;

    if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
        toggleData->debug->toggle();

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        toggleData->player->hasCollision = !toggleData->player->hasCollision;

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        *toggleData->perspective = !(*toggleData->perspective);
        *toggleData->lookChanged = true;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        hasNormalShader = !hasNormalShader;
        if (hasNormalShader)
            toggleData->minecraft->chunkShader = toggleData->minecraft->normalChunkShader;
        else
            toggleData->minecraft->chunkShader = toggleData->minecraft->wireframeChunkShader;
        toggleData->minecraft->initUniforms(toggleData->player->camera);
    }

}

Event::Event() {
    window = 0;
    mousePos = glm::dvec2(0);
    positionChanged = false;
    lookChanged = false;
    inMenu = true;
    perspective = NORMAL_PERSPECTIVE;
    speed = 0.4f;
    mouseSensitivity = 0.1f;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

Event::~Event() {
    free(glfwGetWindowUserPointer(window));
}

void Event::Link(GLFWwindow* window, Debug *debug, Player *player, Minecraft *minecraft) {
    this->window = window;
    this->player = player;

    ToggleData* toggleData = (ToggleData*)calloc(1, sizeof(ToggleData));
    if (!toggleData)
        exit(1);
    toggleData->debug = debug;
    toggleData->player = player;
    toggleData->minecraft = minecraft;
    toggleData->lookChanged = &this->lookChanged;
    toggleData->perspective = &this->perspective;
    glfwSetWindowUserPointer(window, toggleData);
    glfwSetKeyCallback(window, keyToogleCallback);
}

glm::vec3  Event::spectatorMovement() {
    glm::vec3 newPos = glm::vec3(0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += player->look;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos += -glm::normalize(glm::cross(player->look, glm::vec3(0.0f, 1.0f, 0.0f)));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos += -player->look;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += glm::normalize(glm::cross(player->look, glm::vec3(0.0f, 1.0f, 0.0f)));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        newPos.y += -1.0f;
    return newPos;
}

void Event::MovementEvent() {
    glm::vec3 newPos = glm::vec3(0);
    glm::vec3 oldPos = player->position;
    glm::vec3 look = glm::normalize(glm::vec3(player->look.x, 0, player->look.z));

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 10.0f;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        speed = 0.4f;
    if (!player->hasCollision)
        newPos = spectatorMovement();
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            newPos += look;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            newPos += -glm::normalize(glm::cross(look, glm::vec3(0.0f, 1.0f, 0.0f)));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            newPos += -look;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            newPos += glm::normalize(glm::cross(look, glm::vec3(0.0f, 1.0f, 0.0f)));
        newPos.y = 0;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            newPos.y = 1.0f;
        if (!newPos.y)
            newPos.y -= 1.0f;
    }
    newPos *= speed * frequence;
    player->Move(newPos);
    if (player->position != oldPos) {
        player->Update();
        positionChanged = true;
    }
}

void Event::MouseEvent() {
    int mouseState;
    double posx, posy;

    mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (inMenu == true) {
        if (mouseState == GLFW_PRESS) {
            glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
            inMenu = false;
        }
        return;
    }
    glfwGetCursorPos(window, &posx, &posy);
    if (posx != mousePos.x || posy != mousePos.y)
        lookChanged = true;

    Pitch += (float)(posy - mousePos.y);
    Yaw += (float)(posx - mousePos.x);
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
       Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = -sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    player->look = glm::normalize(front);
    player->Update();
    mousePos.x = posx;
    mousePos.y = posy;
}

void Event::GetEvents() {
    lookChanged = false;
    positionChanged = false;
    glfwPollEvents();
    MovementEvent();
    MouseEvent();
    if (positionChanged || lookChanged) {
        if (perspective)
            player->camera.view = glm::lookAt(glm::vec3(player->position.x , 300, player->position.z),
                glm::vec3(player->position.x + 1, 60, player->position.z), glm::vec3(0, 1, 0));
        player->selectedCube = rayCastGetCube(player->position, player->look, PLAYER_RANGE);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        removePointedCube();
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        placeCube();
}