#ifndef SHADOW_CLASS_H
#define SHADOW_CLASS_H

#include <glad/glad.h>
#include "shader.h"

#define SHADOW_TEXTURE_SIZE 2048

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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadowShader.Load("shaders/shadowVS.glsl", "shaders/shadowFS.glsl");

        float near_plane = 0.1f, far_plane = 100.0f;
        projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    }

    void GenerateShadowMap(glm::vec3 sunPos, Minecraft &minecraft) {
        glViewport(0, 0, SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 sunMat = glm::lookAt(sunPos + minecraft.camera.position, minecraft.camera.position, glm::vec3(0, 1, 0));
        minecraft.chunkShader.setMat4("projection", projection);
        minecraft.chunkShader.setMat4("view", sunMat);
        sunMat = projection * sunMat;
        shadowShader.Activate();
        shadowShader.setMat4("matrix", sunMat);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_CULL_FACE);
        for (int n = 0; n < minecraft.chunks.size(); n++)
            minecraft.chunks[n]->Draw(shadowShader);
        glDisable(GL_CULL_FACE);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        int width, height;
        glfwGetFramebufferSize(minecraft.window, &width, &height);    // reset the viewport
        glViewport(0, 0, width, height);
    }

    ~Shadow() {
        glDeleteFramebuffers(1, &frameBufferID);
        glDeleteTextures(1, &textureID);
    }
};

#endif