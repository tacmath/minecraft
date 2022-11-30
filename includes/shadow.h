#ifndef SHADOW_CLASS_H
#define SHADOW_CLASS_H

#include <glad/glad.h>
#include "shader.h"

#define SHADOW_TEXTURE_SIZE 4096

class Shadow {
private:
	GLuint textureID;
    GLuint frameBufferID;
    Shader shadowShader;
    glm::mat4 projection;

public:
    Shadow() {
        textureID = 0;
        frameBufferID = 0;
    }

    void Init() {
        glGenTextures(1, &textureID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
             SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
        glActiveTexture(GL_TEXTURE0);

        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadowShader.Load("shaders/shadowVS.glsl", "shaders/shadowFS.glsl");

        projection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, 0.0f, 250.0f);
    }

    void GenerateShadowMap(glm::vec3 sunPos, Minecraft &minecraft) {
        glViewport(0, 0, SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        sunPos.x += minecraft.camera.position.x;
        sunPos.z += minecraft.camera.position.z;
        glm::mat4 sunMat = glm::lookAt(sunPos, glm::vec3(minecraft.camera.position.x, 60.01f, minecraft.camera.position.z), glm::vec3(0, 1, 0));
        sunMat = projection * sunMat;
        minecraft.chunkShader.Activate();
        minecraft.chunkShader.setMat4("lightSpaceMatrix", sunMat);
        shadowShader.Activate();
        shadowShader.setMat4("matrix", sunMat);

        renderChunks(minecraft);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        int width, height;
        glfwGetFramebufferSize(minecraft.window, &width, &height);    // reset the viewport
        glViewport(0, 0, width, height);
    }

    ~Shadow() {
        glDeleteFramebuffers(1, &frameBufferID);
        glDeleteTextures(1, &textureID);
    }

private:
    void renderChunks(Minecraft& minecraft) {
        int maxChunk;
        Chunk *chunk;

        glEnable(GL_CULL_FACE);
        maxChunk = DATA_RENDER_DISTANCE << 1;
        for (int x = 0; x < maxChunk; x++) {
            for (int z = 0; z < maxChunk; z++) {
                chunk = minecraft.loadedChunks[x * maxChunk + z];
                if (!chunk)
                    continue;
                if (x > DATA_RENDER_DISTANCE - 6 && x < DATA_RENDER_DISTANCE + 6
                    && z > DATA_RENDER_DISTANCE - 6 && z < DATA_RENDER_DISTANCE + 6)
                    chunk->Draw(shadowShader);
            }
        }
        glDisable(GL_CULL_FACE);
    }
};

#endif