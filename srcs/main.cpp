#include "minecraft.h"

#include <glm/gtx/string_cast.hpp>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int showFPS(GLFWwindow* window) {
    static double oldTime = 0;
    static double newTime;
    static int frames = 0;
    static char title[50];
    double timeDiff;

    newTime = glfwGetTime();
    timeDiff = newTime - oldTime;
    frames++;
    if (timeDiff < 1.0f / 30.0f)
        return (0);
    sprintf(title, "Minecraft :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
    glfwSetWindowTitle(window, title);
    frames = 0;
    oldTime = newTime;
    return (1);
}

void loop(Minecraft &minecraft) {
//    glfwSwapInterval(0);
    while (1) {

        minecraft.Draw();
     
        glfwSwapBuffers(minecraft.window);

        if (showFPS(minecraft.window)) { // return 1 when at an inteval
            glfwPollEvents();

            if (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
                glfwWindowShouldClose(minecraft.window) == 1)
                return;
            minecraft.camera.Inputs(minecraft.window);
            minecraft.LoadChunks();
            minecraft.LoadViewMatrix();
        }
    }
}



int main(void) {
    Minecraft minecraft;
    
    loop(minecraft);
 /*   minecraft.~Minecraft();
    Chunk newa;
    newa.Init(0, 0);
    free(newa.cubes);
    for (int n = 0; n < minecraft.chunks.size(); n++)
        free(minecraft.chunks[n].cubes);*/
    _CrtDumpMemoryLeaks();      //test les leaks
    
	return (0);
}