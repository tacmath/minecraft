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
#include "motor.h"
#include <iomanip>


// all the globals needed
Block blocks[256];
std::map<int64_t, Chunk*> chunksMap;

ChunkGeneration     globalChunkGeneration;

void loop(Minecraft &minecraft) {
    Debug           debug;
    UserInterface   UI;
    Motor           motor;
    Event           event;

    debug.Init(&minecraft.windowSize, &minecraft.player, &minecraft.camera, minecraft.window);

    UI.Init(minecraft.windowSize);
    UI.InitUniforms(minecraft.camera.projection);
    UI.SetViewMatrix(minecraft.camera.view);

    event.Init(minecraft.window);

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
            if (motor.update(time)) {
                minecraft.LoadChunks();
                minecraft.thread.BindAllChunks();
                minecraft.thread.UnlockLoadedChunks();
            }
            minecraft.Draw();
            UI.Draw(minecraft);
            debug.fpsTitle(time, latence);
            debug.Draw(time, latence);

            glfwSwapBuffers(minecraft.window);
            previousFrameTime = time;
            previousLoopTime = time - (diff - maxfps);
        }
    }
}

int main(void) {
    Minecraft       minecraft;

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


