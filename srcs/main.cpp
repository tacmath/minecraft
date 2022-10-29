#include "minecraft.h"

#include <glm/gtx/string_cast.hpp>


int showFPS(GLFWwindow* window) {
    static double oldTime = 0;
    static double newTime;
    static int frames = 0;
    static char title[40];
    double timeDiff;

    newTime = glfwGetTime();
    timeDiff = newTime - oldTime;
    frames++;
    if (timeDiff < 1.0f / 30.0f)
        return (0);
    sprintf(title, "scop :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
    glfwSetWindowTitle(window, title);
    frames = 0;
    oldTime = newTime;
    return (1);
}

void loop(Minecraft minecraft) {
    glfwSwapInterval(0);
    while (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(minecraft.window) == 0) {
        glfwPollEvents();

        minecraft.Draw();
     
        glfwSwapBuffers(minecraft.window);
        if (showFPS(minecraft.window)) { // return 1 when at an inteval
            minecraft.camera.Inputs(minecraft.window);
            minecraft.LoadViewMatrix();
        }
    }
}

int main(void) {
    Minecraft minecraft;
    Texture texDirt("texture/dirt.png", 0);
    
    loop(minecraft);
	return (0);
}