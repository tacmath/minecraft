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

#define JUMP_TIME       0.15f
#define JUMP_HEIGHT     1.2f
#define JUMP_GRAVITY    -2 * JUMP_HEIGHT / (JUMP_TIME * JUMP_TIME)
#define JUMP_V0         2 * JUMP_HEIGHT / JUMP_TIME

void Event::MovementEvent(float latency) {
    glm::vec3 newPos = glm::vec3(0);
    glm::vec3 oldPos = player->position;
    glm::vec3 look = glm::normalize(glm::vec3(player->look.x, 0, player->look.z));

    static float startTime = 0;
    static int jumping = 0;

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
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (oldPos.y > 1 && (float)glfwGetTime() - startTime > JUMP_TIME
                && player->Grounded()) {
                jumping = 1;
                startTime = (float)glfwGetTime();
            }
        }
        if (jumping) { // TODO change how the jump is made and where, it should be on the entity side not event
            float time = (float)glfwGetTime() - startTime;
            newPos.y += 0.5f * JUMP_GRAVITY * time * time + JUMP_V0 * time;
            if (time > 0.35f)
               jumping = 0;
        }
        else if (!newPos.y)
            newPos.y -= 1.0f; // a remplacer par la vrai formule de graviter pour une persone de 75 kg
    }
    newPos *= speed * latency;
    player->Move(newPos); // impl�menter la graviter et le jump dans move ou cr�e une fonction apply gavity 
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