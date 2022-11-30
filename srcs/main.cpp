#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "minecraft.h"
#include "chunk_generation.h"
#include "perlinNoise.h"
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
std::map<int64_t, Chunk*> chunksMap;

ChunkGeneration     globalChunkGeneration;


Event           event;
Minecraft       minecraft;
Debug           debug = Debug(minecraft.windowsSize.x, minecraft.windowsSize.y);
UserInterface   UI = UserInterface(minecraft.windowsSize.x, minecraft.windowsSize.y);

void sun(Minecraft& minecraft, Event& event) {
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

            event.frequence = latence / 33.33f;
            event.GetEvents(minecraft.camera, minecraft.player, debug);
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
            if (minecraft.motor.update(time)) {
                minecraft.LoadChunks();
                minecraft.thread.BindAllChunks();
                minecraft.thread.UnlockLoadedChunks();
                sun(minecraft, event);
            }
            minecraft.Draw();
            UI.Draw(minecraft);
            debug.fpsTitle(time, latence, minecraft);
            debug.Draw(time, latence, minecraft);

            glfwSwapBuffers(minecraft.window);
            previousFrameTime = time;
            previousLoopTime = time - (diff - maxfps);
        }
    }
}

int main(void) {
    UI.InitUniforms(minecraft.camera.projection);
    UI.SetViewMatrix(minecraft.camera.view);
    event.Init(minecraft.window);
    glfwSetWindowTitle(minecraft.window, "Minecraft");
    glfwSwapInterval(0);
    loop(minecraft);
    minecraft.thread.StopThreads();
	return (0);
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


