﻿#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "minecraft.h"
#include "chunk_generation.h"
#include "perlin_noise.h"
#include "blocks.h"
#include "event.h"
#include "UI.h"
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "debug.h"
#include <iomanip>

#include "debug.h"
#include <iomanip>


// all the globals needed
Block blocks[256];
ChunkGeneration globalChunkGeneration;
std::map<int64_t, Chunk*> chunksMap;
Noise global_noise;

Minecraft minecraft;
Debug debug = Debug(minecraft.windowsSize.x, minecraft.windowsSize.y);
UserInterface UI = UserInterface(minecraft.windowsSize.x, minecraft.windowsSize.y);

void loop(Minecraft &minecraft) {
    float previousLoopTime = 0;
    float previousFrameTime = 0;
    float diff = 0;
    float time = 0;
    float latence = 0;
    float maxfps = (1.0f / MAX_FPS);
    while  (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(minecraft.window) != 1) {
        time = (float)glfwGetTime();
        diff = time - previousLoopTime;
        if (diff >= maxfps) {
            latence = ((time - previousFrameTime) * 1000);

            minecraft.event.frequence = latence / 33.33f;
            minecraft.event.GetEvents(minecraft.camera);
            if (minecraft.event.lookChanged || minecraft.event.positionChanged) {
                minecraft.setChunksVisibility();
                minecraft.LoadViewMatrix();
                UI.SetViewMatrix(minecraft.camera.view);
            }
            minecraft.LoadChunks();
            minecraft.thread.BindAllChunks();
            minecraft.thread.UnlockLoadedChunks();
            minecraft.Draw();
            UI.SetHighlight(minecraft.player.selectedCube);
            UI.DrawHighlight();
            UI.DrawCross(minecraft.windowsSize.x / 2, minecraft.windowsSize.y / 2);
            debug.display(time, latence, minecraft);

            glfwSwapBuffers(minecraft.window);
            previousFrameTime = time;
            previousLoopTime = time - (diff - maxfps);
        }
    }
}
/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/   


int main(void) {
    glfwSetWindowSizeCallback(minecraft.window, [](GLFWwindow* window, int width, int height)
    {
            std::cout << "RESIZE" << std::endl;
            minecraft.windowsSize = glm::vec2((float)width, (float)height);
            minecraft.camera.updateSize((float)width, (float)height);
            debug.setProjection((float)width, (float)height);
            UI.setTextProjection((float)width, (float)height);
            glViewport(0, 0, (int)minecraft.windowsSize.x,  (int)minecraft.windowsSize.y);
    });

//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    UI.InitUniforms(minecraft.camera.projection);
    UI.SetViewMatrix(minecraft.camera.view);
    minecraft.event.Init(minecraft.window);
    glfwSetWindowTitle(minecraft.window, "Minecraft");
    glfwSwapInterval(0);
    loop(minecraft);
    minecraft.thread.StopThreads();
	return (0);
}
