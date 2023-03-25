#include "chunk.h"
#include "camera.h"
#include "event.h"
#include "raycast.h"

void removePointedCube(Player &player, Cooldowns &cooldowns) {
    Chunk* chunk;
    glm::ivec3 pos;

    if (player.selectedCube.id == AIR || !cooldowns.Use(ACTION))
        return;
    pos = player.selectedCube.position;
    chunk = GetChunk(pos.x >> 4, pos.z >> 4);
    chunk->SetCube(AIR, pos.x & 0xF, pos.y & 0xFF, pos.z & 0xF);
    chunk->UpdateCube(pos.x & 0xF, pos.z & 0xF);
    player.UpdateRayCast();
}

void placeCube(Player &player, Cooldowns &cooldowns) { // maybe place the function inside the player class
    Chunk* chunk;
    glm::ivec3 pos;

    if (player.selectedCube.id == AIR || player.selectedCube.range == 0 ||
        player.aabb().collide(AABB::unit().translate(player.selectedCube.side)) || !cooldowns.Use(ACTION))
        return;
    pos = player.selectedCube.side;
    chunk = GetChunk(pos.x >> 4, pos.z >> 4);
    chunk->SetCube(player.selectedItem, pos.x & 0xF, pos.y & 0xFF, pos.z & 0xF);
    chunk->UpdateCube(pos.x & 0xF, pos.z & 0xF);
    player.UpdateRayCast();
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    static GlfwCallbackData* data = (GlfwCallbackData*)glfwGetWindowUserPointer(window);

    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        data->player->selectedItem = data->player->selectedCube.id;

    if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
        data->cooldowns->Reset(ACTION);
}


void keyToogleCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static GlfwCallbackData* data = (GlfwCallbackData*)glfwGetWindowUserPointer(window);
    static bool wireFrameMode = false;
    static bool fullScreen = false;
    static bool shadow = true;

    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        if (fullScreen)
            data->window->Windowed();
        else
            data->window->FullScreen();
        fullScreen = !fullScreen;
    }
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        std::vector<std::string> shaderOption;
        shadow = !shadow;

        if (shadow) {
            shaderOption.push_back("SHADOW");
            data->shadow->Activate();
        }
        else {
            data->shadow->Delete();
        }
        data->worldArea->ReloadShader(wireFrameMode, shaderOption);
        data->worldArea->initUniforms(data->player->camera);
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
        data->menu->toggle();
    if (key == GLFW_KEY_V && action == GLFW_PRESS && data->menu->status & DEBUG_ON)
        data->menu->toggleView();

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        data->player->hasCollision = !data->player->hasCollision;

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        *data->perspective = !(*data->perspective);
        *data->lookChanged = true;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        std::vector<std::string> shaderOption;
        if (shadow)
            shaderOption.push_back("SHADOW");

        wireFrameMode = !wireFrameMode;
        data->worldArea->ReloadShader(wireFrameMode, shaderOption);
        data->worldArea->initUniforms(data->player->camera);
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        data->player->selectedItem = 1;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        data->player->selectedItem = 2;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        data->player->selectedItem = 3;
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        data->player->selectedItem = 4;
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        data->player->selectedItem = 5;
}

Event::Event() {
    window = 0;
    mousePos = glm::dvec2(0);
    playerUpdated = false;
    inMenu = true;
    perspective = NORMAL_PERSPECTIVE;
    speed = 10.0f;
    mouseSensitivity = 0.5f;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

Event::~Event() {
    free(glfwGetWindowUserPointer(window));
}

void Event::Link(Window* window, Menu *menu, Player *player, WorldArea* worldArea, Cooldowns* cooldowns, Shadow *shadow) {
    this->window = window->context;
    this->player = player;
    this->cooldowns = cooldowns;

    GlfwCallbackData* toggleData = (GlfwCallbackData*)calloc(1, sizeof(GlfwCallbackData));
    if (!toggleData)
        exit(1);
    toggleData->menu = menu;
    toggleData->player = player;
    toggleData->worldArea = worldArea;
    toggleData->window = window;
    toggleData->shadow = shadow;
    toggleData->cooldowns = cooldowns;
    toggleData->lookChanged = &this->playerUpdated;
    toggleData->perspective = &this->perspective;
    toggleData->windowSizeCallback = [](int width, int height) {(void)width;(void)height;};
    glfwSetWindowUserPointer(this->window, toggleData);
    glfwSetKeyCallback(this->window, keyToogleCallback);
    glfwSetMouseButtonCallback(this->window, mouseButtonCallback);
    glfwSetWindowSizeCallback(this->window, [](GLFWwindow* window, int width, int height) {
        static GlfwCallbackData* toggleData = (GlfwCallbackData*)glfwGetWindowUserPointer(window);
        
        toggleData->windowSizeCallback(width, height);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    });
}

void Event::SetWindowSizeCallback(std::function<void(int width, int height)> windowSizeCallback) {
    GlfwCallbackData* toggleData = (GlfwCallbackData*)glfwGetWindowUserPointer(window);
    toggleData->windowSizeCallback = windowSizeCallback;
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

void Event::MovementEvent(float latency) {
    glm::vec3 newPos = glm::vec3(0);
    glm::vec3 oldPos = player->position;
    glm::vec3 look = glm::normalize(glm::vec3(player->look.x, 0, player->look.z));

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 160.0f;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        speed = 10.0f;
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
    newPos *= speed * latency;
    player->Move(newPos);
    if (player->position != oldPos)
        playerUpdated = true;
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
        playerUpdated = true;

    Pitch += (float)(posy - mousePos.y) * mouseSensitivity;
    Yaw += (float)(posx - mousePos.x) * mouseSensitivity;
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
       Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = -sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    player->look = glm::normalize(front);
    mousePos.x = posx;
    mousePos.y = posy;
}

void Event::GetEvents(float latency) {
    playerUpdated = false;
    glfwPollEvents();
    MovementEvent(latency);
    MouseEvent();
    if (playerUpdated) {
        player->Update();
        if (perspective) {
            player->camera.view = glm::lookAt(glm::vec3(player->position.x, 300, player->position.z),
                glm::vec3(player->position.x + 1, 60, player->position.z), glm::vec3(0, 1, 0));
            player->UpdateCallback();
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        removePointedCube(*player, *cooldowns);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        placeCube(*player, *cooldowns);
}