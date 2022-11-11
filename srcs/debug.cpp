#include "debug.h"

Debug::Debug(float width, float height) {
    text.Init(width, height);
    diff = 0;
    fps[0] = 0;
    xyz[0] = 0;
    frame = 0;
    visible = true;
    previousUpdateTime = 0.0f;
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

void Debug::display(float time, float latence, Minecraft &minecraft) {
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
    sprintf(xyz, "XYZ: %.3f / %.3f / %.3f", minecraft.camera.position.x, minecraft.camera.position.y, minecraft.camera.position.z);


    Chunk *chunk = GetChunk((int)(minecraft.camera.position.x) >> 4, (int)(minecraft.camera.position.z) >> 4);
    if (chunk != 0) {
        sprintf(xz, "Chunk: %d %d", chunk->posx, chunk->posz);
    }
    else {
        sprintf(xz, "Chunk: undefined");
    }

    sprintf(target, "target: %d %d %d", minecraft.event.selectedCube.x, minecraft.event.selectedCube.y, minecraft.event.selectedCube.z);

    double y = minecraft.windowsSize.y - 15;
    double scale = y / 600 / 5;
    double space = scale * 100.0f;
    text.display("Vox Version 0.1", 5.0f, y, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(fps, 5.0f, y - space * 1, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xyz, 5.0f, y - space * 2, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xz, 5.0f, y - space * 3, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(target, 5.0f, y - space * 4, scale, glm::vec3(1.0, 1.0f, 1.0f));
}