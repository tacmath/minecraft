#include "debug.h"

Debug::Debug() {
    diff = 0;
    fps[0] = 0;
    xyz[0] = 0;
    frame = 0;
    visible = false;
    previousUpdateTime = 0.0f;
}

void Debug::Link(glm::vec2 *windowSize, Player *player, GLFWwindow *window) {
    this->windowSize = windowSize;
    this->player = player;
    this->window = window;
    text.Init(windowSize->x, windowSize->y);
}

void Debug::toggle() {
    if (visible == true) {
        disable();
        return;
    }
    enable();
}

void Debug::enable() {
    visible = true;
}

void Debug::disable() {
    visible = false;
    previousUpdateTime = 0;
    frame = 0;
}

void Debug::setProjection(float width, float height) {
    text.setProjection(width, height);
}

void Debug::fpsTitle(float time, float latence) {
    static char title[100];

    frame_title += 1;
    diff = time - previousUpdateTime_title;
    if (diff >= 0.2f) {
        sprintf(title, "Minecraft :  FPS: %d (%.2f ms)", frame_title * 5, latence);
        glfwSetWindowTitle(window, title);
        previousUpdateTime_title = time - (diff - 0.2f);
        frame_title = 0;
    }
}

void Debug::Draw(float time, float latence) {
    if (visible == false) return;
    frame += 1;

    diff = time - previousUpdateTime;
    if (diff >= 0.2f) {
        if (previousUpdateTime > 10.0) {
            sprintf(fps, "FPS: %d  (max %d / latence: %.2f ms)", frame * 5, MAX_FPS, latence);
        }
        else {
            sprintf(fps, "FPS: calculate...");
        }
        frame = 0;
        if (diff >= 0.4) diff = 0.0f;
        previousUpdateTime = time - (diff - 0.2f);
    }
    sprintf(xyz, "XYZ: %.3f / %.3f / %.3f", player->position.x, player->position.y, player->position.z);


    Chunk *chunk = GetChunk((int)(player->position.x) >> 4, (int)(player->position.z) >> 4);
    if (chunk != 0) {
        sprintf(xz, "Chunk: %d %d", chunk->posx, chunk->posz);
    }
    else {
        sprintf(xz, "Chunk: undefined");
    }

    sprintf(target, "target: %d %d %d", player->selectedCube.position.x, player->selectedCube.position.y, player->selectedCube.position.z);

    float y = windowSize->y - 15;
    float scale = (y / 600) / 5;
    float space = scale * 100.0f;
    text.display("Vox Version 0.1", 5.0f, y, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(fps, 5.0f, y - space * 1, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xyz, 5.0f, y - space * 2, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xz, 5.0f, y - space * 3, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(target, 5.0f, y - space * 4, scale, glm::vec3(1.0, 1.0f, 1.0f));
}