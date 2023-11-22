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
    Chunk::blocks[player.selectedCube.id].PlayBreakSound((float)pos.x + 0.5f, (float)pos.y + 0.5f, (float)pos.z + 0.5f);
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
    Chunk::blocks[(int)player.SelectedItem()].PlayBreakSound((float)pos.x + 0.5f, (float)pos.y + 0.5f, (float)pos.z + 0.5f);
    chunk = GetChunk(pos.x >> 4, pos.z >> 4);
    chunk->SetCube(player.SelectedItem(), pos.x & 0xF, pos.y & 0xFF, pos.z & 0xF);
    chunk->UpdateCube(pos.x & 0xF, pos.z & 0xF);
    player.UpdateRayCast();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    static Player* player = ((GlfwCallbackData*)glfwGetWindowUserPointer(window))->player;
    (void)xoffset;

    if (yoffset < 0)
        player->SetSelectedSlot((player->GetSelectedSlot() + 1) * (player->GetSelectedSlot() < INVENTORY_SIZE - 1));
    else if (yoffset > 0)
        player->SetSelectedSlot(((player->GetSelectedSlot() > 0) ? player->GetSelectedSlot() - 1 : INVENTORY_SIZE - 1));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    static GlfwCallbackData* data = (GlfwCallbackData*)glfwGetWindowUserPointer(window);

    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        data->player->SetInventoryItem(data->player->GetSelectedSlot(), data->player->selectedCube.id);

    if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
        data->cooldowns->Reset(ACTION);
}


void keyToogleCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static GlfwCallbackData* data = (GlfwCallbackData*)glfwGetWindowUserPointer(window);

    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
        data->menu->Toogle();
}

Event::Event() {
    window = 0;
    mousePos = glm::dvec2(0);
    playerUpdated = false;
    inMenu = true;
    speed = 10.0f;
    mouseSensitivity = 0.5f;
    Yaw = -90.0f;
    Pitch = 0.0f;
    cooldowns = NULL;
    menu = NULL;
    player = NULL;
}

Event::~Event() {
    free(glfwGetWindowUserPointer(window));
}

void Event::Link(Window* window, Menu *menu, Player *player, Cooldowns* cooldowns) {
    this->window = window->context;
    this->player = player;
    this->cooldowns = cooldowns;
    this->menu = menu;

    GlfwCallbackData* toggleData = (GlfwCallbackData*)calloc(1, sizeof(GlfwCallbackData));
    if (!toggleData)
        exit(1);
    toggleData->menu = menu;
    toggleData->player = player;
    toggleData->cooldowns = cooldowns;
    toggleData->windowSizeCallback = [](int width, int height) {(void)width;(void)height;};
    glfwSetWindowUserPointer(this->window, toggleData);
    glfwSetKeyCallback(this->window, keyToogleCallback);
    glfwSetMouseButtonCallback(this->window, mouseButtonCallback);
    glfwSetScrollCallback(this->window, scroll_callback);
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
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 oldPos = player->position;
    glm::vec3 look = glm::normalize(glm::vec3(player->look.x, 0, player->look.z));

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 160.0f;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        speed = 10.0f;
    if (!player->hasCollision)
        velocity = spectatorMovement();
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            velocity += look;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            velocity += -glm::normalize(glm::cross(look, glm::vec3(0.0f, 1.0f, 0.0f)));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            velocity += -look;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            velocity += glm::normalize(glm::cross(look, glm::vec3(0.0f, 1.0f, 0.0f)));
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            player->Jump();
    }
    velocity *= speed;
    player->Move(velocity, latency);
    if (player->position != oldPos)
        playerUpdated = true;
}

void Event::MouseEvent() { // TODO rework and use Pitch and Yaw in camera
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
    if (menu->IsOpen()) { //maybe just switch the callbacks instead when in menu
        glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
        if (playerUpdated)
            player->Update();
        return;
    }
    MouseEvent();
    if (playerUpdated)
        player->Update();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        removePointedCube(*player, *cooldowns);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        placeCube(*player, *cooldowns);
}