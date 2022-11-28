#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "minecraft.h"
#include "chunk_generation.h"
#include "blocks.h"
#include "event.h"
#include "UI.h"

// all the globals needed
Block blocks[256];
ChunkGeneration globalChunkGeneration;
std::map<int64_t, Chunk*> chunksMap;
Noise global_noise;

void getMouseEvent(GLFWwindow* window, Camera& camera);

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
    sprintf(title, "Minecraft :  FPS = %d  ms = %f coor = %f %f %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames, minecraft.camera.position.x, minecraft.camera.position.y, minecraft.camera.position.z);
    glfwSetWindowTitle(window, title);
    frames = 0;
    oldTime = newTime;
    return (1);
}

void sun(Minecraft& minecraft, Event &event) {
    static float time = 0.0f;
    glm::vec3 sunPos;

    time++;
    if (time >= 180.0f)
        time = 0.0f;
    if (!event.sunMode)
        return;
    sunPos.z = 500 * -cos(glm::radians(time));
    sunPos.y = 500 * sin(glm::radians(time));
    sunPos.x = 0;

    glm::mat4 sunMat = glm::lookAt(sunPos + minecraft.camera.position, minecraft.camera.position, glm::vec3(0, 1, 0));
    minecraft.chunkShader.Activate();
    minecraft.chunkShader.setMat4("view", sunMat);
    minecraft.skyboxShader.Activate();
    minecraft.skyboxShader.setMat4("view", glm::mat4(glm::mat3(sunMat)));
}

void loop(Minecraft &minecraft) {
    Event event;
    UserInterface UI;
    bool    hasNormalShader;

    hasNormalShader = true;
    UI.InitUniforms(minecraft.camera.projection);
    UI.SetViewMatrix(minecraft.camera.view);
    event.Init(minecraft.window);
    glfwSwapInterval(0);
    while (1) {

        minecraft.Draw();
        UI.DrawHighlight();
     
        glfwSwapBuffers(minecraft.window);

        if (showFPS(minecraft.window, minecraft)) { // return 1 when at an inteval
            event.GetEvents(minecraft.camera, minecraft.player);
            if (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
                glfwWindowShouldClose(minecraft.window) == 1)
                return;
            if (event.lookChanged || event.positionChanged) {
                minecraft.setChunksVisibility();
                minecraft.LoadViewMatrix();
                UI.SetViewMatrix(minecraft.camera.view);
            }
            if (event.chunkShaderChanged) {
                hasNormalShader = !hasNormalShader;
                if (hasNormalShader)
                    minecraft.changeShader(minecraft.chunkShader, minecraft.normalChunkShader);
                else
                    minecraft.changeShader(minecraft.chunkShader, minecraft.wireframeChunkShader);
            }
            UI.SetHighlight(minecraft.player.selectedCube);
            minecraft.LoadChunks();
            minecraft.thread.BindAllChunks();
            minecraft.thread.UnlockLoadedChunks();
            sun(minecraft, event);
        }
    }
}

GLenum glCheckError()
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << std::endl;
    }
    return errorCode;
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