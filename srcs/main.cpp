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
std::unordered_map<int64_t, Chunk*> chunksMap;

ChunkGeneration     globalChunkGeneration;

void loop(Minecraft &minecraft) {
    Player          player;
    Debug           debug;
    UserInterface   UI;
    Motor           motor;
    Event           event;

    player.Init(minecraft.windowSize);
    minecraft.initUniforms(player.camera);

    debug.Link(&minecraft.windowSize, &player, minecraft.window);

    UI.Link(&minecraft.windowSize);
    UI.InitUniforms(player.camera.projection);
    UI.SetViewMatrix(player.camera.view);

    event.Link(minecraft.window, &debug, &player, &minecraft);

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
            event.GetEvents();
            if (event.lookChanged || event.positionChanged) {
                minecraft.setChunksVisibility(player.camera);
                minecraft.LoadViewMatrix(player.camera);
                UI.SetViewMatrix(player.camera.view);
            }

            if (motor.update(time)) {
                minecraft.LoadChunks(player.position, player.camera);
                minecraft.thread.BindAllChunks();
                minecraft.thread.UnlockLoadedChunks();
            }
            minecraft.Draw();
            UI.Draw(player);
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


