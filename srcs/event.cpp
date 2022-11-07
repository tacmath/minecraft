#include "chunk.h"
#include "camera.h"
#include "event.h"

#define PLAYER_RANGE 5

glm::ivec3 getPointedCube(Camera &camera) {     //test the function
    Chunk* chunk;
    glm::vec3 origin = camera.position - 0.5f;

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

Event::Event() {
    window = 0;
    mousePos = glm::dvec2(0);
    memset(keyPressed, 0, 256);
    positionChanged = false;
    lookChanged = false;
    inMenu = true;
    perspective = NORMAL_PERSPECTIVE;
    speed = 0.4f;
    mouseSensitivity = 0.1f;
    std::cout << "construct" << std::endl;
}

void Event::Init(GLFWwindow* window) {
    this->window = window;
}

void Event::MovementEvent(Camera& camera) {
    glm::vec3 newPos = camera.position;
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
    if (newPos != camera.position) {
        camera.position = newPos;
        positionChanged = true;
    }
}

void Event::KeyEvent() {
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
}

void Event::MouseEvent(Camera &camera) {
    int mouseState;
    double posx, posy;
    float rotx, roty;

    mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouseState == GLFW_PRESS)
        removePointedCube(camera);

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

void Event::GetEvents(Camera& camera) {
    lookChanged = false;
    positionChanged = false;
    glfwPollEvents();
    MovementEvent(camera);
    KeyEvent();
    MouseEvent(camera);
    if (positionChanged || lookChanged)
        camera.Update(perspective);
}