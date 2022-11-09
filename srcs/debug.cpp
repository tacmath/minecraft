#include "debug.h"

Debug::Debug() {
    text.Init();
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

    text.display("Vox Version 0.1", 5.0f, 590.0f, 0.2f, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(fps, 5.0f, 575.0f, 0.2f, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xyz, 5.0f, 560.0f, 0.2f, glm::vec3(1.0, 1.0f, 1.0f));
    
}