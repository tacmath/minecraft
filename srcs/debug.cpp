#include "debug.h"

Debug::Debug() {
    diff = 0;
    fps[0] = 0;
    xyz[0] = 0;
    frame = 0;
    status = DEBUG_OFF;
    previousUpdateTime = 0.0f;
}

void Debug::Link(glm::vec2 *windowSize, Player *player, GLFWwindow *window) {
    this->windowSize = windowSize;
    this->player = player;
    this->window = window;
    text.Init(windowSize->x, windowSize->y);
}

void Debug::toggle() {
    if (status & DEBUG_ON) {
        disable();
        return;
    }
    enable();
}

void Debug::toggleView() {
    if (status & DEBUG_VIEW) {
        status ^= DEBUG_VIEW;
        quad.Delete();
        quadShader.Delete();
        return;
    }

    status |= DEBUG_VIEW;
    quad.Init(glm::vec3(0.0f), glm::vec3(0.8f));
    quadShader.Load("shaders/debugTextureVS.glsl", "shaders/debugTextureFS.glsl");
}

void Debug::enable() {
    status |= DEBUG_ON;
}

void Debug::disable() {
    status ^= DEBUG_ON;
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
        sprintf(title, "Minecraft :  FPS: %d (%.3f ms)", frame_title * 5, latence * 1000);
        glfwSetWindowTitle(window, title);
        previousUpdateTime_title = time - (diff - 0.2f);
        frame_title = 0;
    }
}

void Debug::DrawViews() {
    glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(-0.9f, 0.1f, 0));

    glDisable(GL_DEPTH_TEST);
    quadShader.Activate();
    quadShader.setInt("depthMap", 3);
    quadShader.setInt("index", 0);
    quadShader.setMat4("matrix", matrix);
    quad.Render();
    quadShader.setInt("index", 1);
    quadShader.setMat4("matrix", glm::translate(matrix, glm::vec3(1.0f, 0, 0)));
    quad.Render();
    quadShader.setInt("index", 2);
    quadShader.setMat4("matrix", glm::translate(matrix, glm::vec3(0, -1.0f, 0)));
    quad.Render();
    glEnable(GL_DEPTH_TEST);
}

void Debug::Draw(float time, float latence) {
    if (!(status & DEBUG_ON)) return;

    if (status & DEBUG_VIEW) {
        DrawViews();
        return;
    }

    frame += 1;
    diff = time - previousUpdateTime;
    if (diff >= 0.2f) {
        if (previousUpdateTime > 10.0) {
            sprintf(fps, "FPS: %d  (max %d / latence: %.3f ms)", frame * 5, MAX_FPS, latence * 1000.0f);
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