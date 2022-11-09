﻿#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "minecraft.h"
#include "chunk_generation.h"
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

void loop(Minecraft &minecraft) {
    Event event;
    UserInterface UI;
    Debug debug;
    

    float previousLoopTime = 0;
    float previousFrameTime = 0;
    float diff = 0;
    float time = 0;
    float latence = 0;
    float maxfps = (1.0f / MAX_FPS);

    UI.InitUniforms(minecraft.camera.projection);
    UI.SetViewMatrix(minecraft.camera.view);
    event.Init(minecraft.window);
    glfwSetWindowTitle(minecraft.window, "Minecraft");
    glfwSwapInterval(0);     
    while  (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(minecraft.window) != 1) {
        time = (float)glfwGetTime();
        diff = time - previousLoopTime;
        if (diff >= maxfps) {
            latence = ((time - previousFrameTime) * 1000);

            event.frequence = latence / 33.33f;
            event.GetEvents(minecraft.camera);
            if (event.lookChanged || event.positionChanged) {
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
           // debug.display(time, latence, minecraft);

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
    Minecraft minecraft;
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    loop(minecraft);
    minecraft.thread.StopThreads();
	return (0);
}




