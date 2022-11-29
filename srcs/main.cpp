#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "minecraft.h"
#include "shadow.h"
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

void setChunkVisibility(Minecraft& minecraft) {
    int maxChunk;
    Chunk *chunk;

    maxChunk = DATA_RENDER_DISTANCE << 1;
    //add all the chunk that are in loadedChunks but didn't exist
    for (int n = 0; n < minecraft.chunks.size(); n++)
        minecraft.chunks[n]->isVisible = false;
    for (int x = 0; x < maxChunk; x++)
        for (int z = 0; z < maxChunk; z++) {
            chunk = minecraft.loadedChunks[x * maxChunk + z];
            if (!chunk)
                continue;
            if (x > DATA_RENDER_DISTANCE - 6 && x < DATA_RENDER_DISTANCE + 6
                && z > DATA_RENDER_DISTANCE - 6 && z < DATA_RENDER_DISTANCE + 6)
                chunk->isVisible = true;
        }
}

void sun(Minecraft& minecraft, Event &event, Shadow &shadow) {
    static float time = 0.0f;
    glm::vec3 sunPos;

    time++;
    if (time >= 180.0f)
        time = 0.0f;
    if (!event.sunMode)
        return;
    sunPos.z = 120 * -cos(glm::radians(time));
    sunPos.y = 120 * sin(glm::radians(time));
    sunPos.x = 0;

    setChunkVisibility(minecraft);
    shadow.GenerateShadowMap(sunPos, minecraft);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void loop(Minecraft &minecraft) {
    Event event;
    Shadow shadow;
    UserInterface UI;
    bool    hasNormalShader;
    
    Shader debugShader;


    debugShader.Load("shaders/debugTextureVS.glsl", "shaders/debugTextureFS.glsl");
    debugShader.setInt("depthMap", 3);
    hasNormalShader = true;
    UI.InitUniforms(minecraft.camera.projection);
    UI.SetViewMatrix(minecraft.camera.view);
    event.Init(minecraft.window);
    shadow.Init();
    glfwSwapInterval(0);
    while (1) {
       // if (!event.sunMode) {
            minecraft.Draw();
            UI.DrawHighlight();
       // }
       /* else {
            debugShader.Activate();

            renderQuad();
        }*/
     
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
            sun(minecraft, event, shadow);
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