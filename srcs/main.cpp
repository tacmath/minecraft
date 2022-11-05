#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "minecraft.h"


// all the globals needed
Block blocks[256];
ChunkGeneration globalChunkGeneration;
std::map<int64_t, Chunk*> chunksMap;
Noise global_noise;


int showFPS(GLFWwindow* window, Minecraft &minecraft) {
    static double oldTime = 0;
    static double newTime;
    static int frames = 0;
    static char title[100];
    double timeDiff;

    newTime = glfwGetTime();
    timeDiff = newTime - oldTime;
    frames++;
    if (timeDiff < 1.0f / 30.0f)
        return (0);
    sprintf(title, "Minecraft :  FPS = %d  ms = %f coor = %f %f %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames, minecraft.camera.posision.x, minecraft.camera.posision.y, minecraft.camera.posision.z);
    glfwSetWindowTitle(window, title);
    frames = 0;
    oldTime = newTime;
    return (1);
}

void loop(Minecraft &minecraft) {
    glfwSwapInterval(0);
    while (1) {

        minecraft.Draw();
     
        glfwSwapBuffers(minecraft.window);

        if (showFPS(minecraft.window, minecraft)) { // return 1 when at an inteval
            glfwPollEvents();

            if (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
                glfwWindowShouldClose(minecraft.window) == 1)
                return;
            minecraft.camera.Inputs(minecraft.window);
            minecraft.LoadViewMatrix();
            minecraft.LoadChunks();
            minecraft.thread.BindAllChunks();
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