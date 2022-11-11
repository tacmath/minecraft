#include "chunk.h"
#include "camera.h"
#include "event.h"

#define PLAYER_RANGE 5

//http://www.cse.yorku.ca/~amana/research/grid.pdf
glm::ivec3 getPointedCube(Camera &camera) {     //test the function
    Chunk* chunk;
    glm::vec3 origin = camera.position;
    origin.x -= 0.9f;
    origin.z -= 0.9f;

    for (int n = 0; n < PLAYER_RANGE; n++) {
        chunk = GetChunk((int)origin.x >> 4, (int)origin.z >> 4);   // change later to prevent getting the chunk on every block
        if (chunk && chunk->GetCube((int)origin.x & 0xF, (int)origin.y & 0xFF, (int)origin.z & 0xF) != AIR) {
            return (glm::ivec3(origin));
        }
        origin += camera.direction;
    }
    return (glm::ivec3(-1));
}

void Event::removePointedCube(Camera& camera) {
    Chunk* chunk;
    if (selectedCube.y < 0)
        return;
    chunk = GetChunk(selectedCube.x >> 4, selectedCube.z >> 4);
    chunk->SetCube(AIR, selectedCube.x & 0xF, selectedCube.y & 0xFF, selectedCube.z & 0xF);
    chunk->UpdateCube(selectedCube.x & 0xF, selectedCube.z & 0xF);
    selectedCube = getPointedCube(camera);
}

Event::Event() {
    window = 0;
    mousePos = glm::dvec2(0);
    selectedCube = glm::ivec3(-1);
    memset(keyPressed, 0, 256);
    positionChanged = false;
    lookChanged = false;
    inMenu = true;
    perspective = NORMAL_PERSPECTIVE;
    speed = 0.4f;
    mouseSensitivity = 0.1f;
}

void Event::Init(GLFWwindow* window) {
    this->window = window;
}

glm::vec3  Event::spectatorMovement(Camera& camera, Player& player) {
    glm::vec3 newPos = glm::vec3(0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += speed * camera.direction;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos += speed * -glm::normalize(glm::cross(camera.direction, camera.up));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos += speed * -camera.direction;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += speed * glm::normalize(glm::cross(camera.direction, camera.up));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos += speed * camera.up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        newPos += speed * -camera.up;
    return newPos;
}

void Event::MovementEvent(Camera& camera, Player& player) {
    glm::vec3 newPos = glm::vec3(0);
    if (!player.hasCollision)
        newPos = spectatorMovement(camera, player);
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            newPos += speed * camera.direction;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            newPos += speed * -glm::normalize(glm::cross(camera.direction, camera.up));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            newPos += speed * -camera.direction;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            newPos += speed * glm::normalize(glm::cross(camera.direction, camera.up));
        newPos.y = 0;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            newPos.y = speed;
        if (!newPos.y)
            newPos.y -= 1.0f;
    }
    player.Move(newPos);
    if (player.position != camera.position) {
        camera.position = player.position;
        positionChanged = true;
    }
}

void Event::KeyEvent(Player& player) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 10.0f;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        speed = 0.4f;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && keyPressed[GLFW_KEY_P]) {
        perspective = !perspective;
        lookChanged = true;
        keyPressed[GLFW_KEY_P] = 0;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        keyPressed[GLFW_KEY_P] = 1;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && keyPressed[GLFW_KEY_C]) {
        player.hasCollision = !player.hasCollision;
        keyPressed[GLFW_KEY_C] = 0;
    }
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        keyPressed[GLFW_KEY_C] = 1;
}

void Event::MouseEvent(Camera &camera) {
    int mouseState;
    double posx, posy;
    float rotx, roty;

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

    rotx = (float)(posy - mousePos.y);
    roty = (float)(posx - mousePos.x);

    glm::vec3 newOrientation = glm::rotate(camera.direction, glm::radians(-rotx), glm::normalize(glm::cross(camera.direction, camera.up)));

    // Decides whether or not the next vertical Orientation is legal or not
    if (abs(glm::angle(newOrientation, camera.up) - glm::radians(90.0f)) <= glm::radians(85.0f)) //change the system
        camera.direction = glm::normalize(newOrientation);

    // Rotates the Orientation left and right
    camera.direction = glm::rotate(camera.direction, glm::radians(-roty), camera.up);
    mousePos.x = posx;
    mousePos.y = posy;
}

void Event::GetEvents(Camera& camera, Player& player) {
    lookChanged = false;
    positionChanged = false;
    glfwPollEvents();
    MovementEvent(camera, player);
    KeyEvent(player);
    MouseEvent(camera);
    if (positionChanged || lookChanged) {
        camera.Update(perspective);
        selectedCube = getPointedCube(camera);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        removePointedCube(camera);
}