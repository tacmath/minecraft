#include <glm/gtx/string_cast.hpp>
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
SimplexNoise global_simplex_noise;

Minecraft minecraft;
Debug debug = Debug(minecraft.windowsSize.x, minecraft.windowsSize.y);
UserInterface UI = UserInterface(minecraft.windowsSize.x, minecraft.windowsSize.y);

void loop(Minecraft &minecraft) {
    bool hasNormalShader = false;
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
            minecraft.event.GetEvents(minecraft.camera, minecraft.player);
            if (minecraft.event.lookChanged || minecraft.event.positionChanged) {
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
            minecraft.LoadChunks();
            minecraft.thread.BindAllChunks();
            minecraft.thread.UnlockLoadedChunks();
            sun(minecraft, event);
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
