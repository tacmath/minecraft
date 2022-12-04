#ifndef SHADOW_CLASS_H
#define SHADOW_CLASS_H

#include <glad/glad.h>
#include "shader.h"

#include "debug_utils.h"

#define SHADOW_TEXTURE_SIZE 2048

class Shadow {
private:
	GLuint textureID;
    GLuint frameBufferID;
    Shader shadowShader;
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 frustumCorners[8];


    DebugUtils debug;

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

        projection = glm::ortho(-32.0f, 32.0f, -32.0f, 32.0f, -100.0f, 100.0f);

        debug.initRenderFBO(512, 512, 4);
    }

    void GenerateShadowMap(glm::vec3 lightDir, Minecraft &minecraft) {
        glViewport(0, 0, SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        setLightViewProjectionMatrix(lightDir, minecraft);

        renderChunks(minecraft);



        debug.bindRenderFBO();
        minecraft.chunkShader.Activate();
        minecraft.chunkShader.setMat4("projection", projection);
        minecraft.chunkShader.setMat4("view", view);
        debugRenderChunks(minecraft);
        minecraft.changeShader(minecraft.chunkShader, minecraft.chunkShader);


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

    void getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
        const glm::mat4 inv = glm::inverse(proj * view);
        int n;

        n = 0;
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const glm::vec4 pt =
                        inv * glm::vec4(
                            2.0f * x - 1.0f,
                            2.0f * y - 1.0f,
                            2.0f * z - 1.0f,
                            1.0f);
                    frustumCorners[n++] = pt / pt.w;
                }
            }
        }
    }

    void setLightViewProjectionMatrix(glm::vec3 &lightDir, Minecraft& minecraft) {
        glm::mat4 projection1 = glm::perspective(glm::radians(90.0f), (float)(1700.0f / 1080.0f), 0.1f, 20.0f);
        getFrustumCornersWorldSpace(projection1, minecraft.camera.view);

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();

        glm::vec3 center = glm::vec3(0, 60, 0);
        for (int n = 0; n < 8; n++)
            center += glm::vec3(frustumCorners[n]);
        center /= 8;

        view = glm::lookAt(
            center + lightDir,
            center,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        for (int n = 0; n < 8; n++)
        {
            glm::vec4 trf = view * frustumCorners[n];
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        projection = glm::ortho(minX, maxX, minY, maxY, minZ - 128, maxZ + 128);


        glm::mat4 sunMat = projection * view;

        minecraft.chunkShader.Activate();
        minecraft.chunkShader.setVec3("lightDir", lightDir);
        minecraft.chunkShader.setMat4("lightSpaceMatrix", sunMat);

        shadowShader.Activate();
        shadowShader.setMat4("matrix", sunMat);
    }

    void renderChunks(Minecraft& minecraft) {
        int maxChunk;
        Chunk *chunk;

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 1.0f);
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
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    void debugRenderChunks(Minecraft& minecraft) {
        int maxChunk;
        Chunk* chunk;

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 1.0f);
        maxChunk = DATA_RENDER_DISTANCE << 1;
        for (int x = 0; x < maxChunk; x++) {
            for (int z = 0; z < maxChunk; z++) {
                chunk = minecraft.loadedChunks[x * maxChunk + z];
                if (!chunk)
                    continue;
                if (x > DATA_RENDER_DISTANCE - 6 && x < DATA_RENDER_DISTANCE + 6
                    && z > DATA_RENDER_DISTANCE - 6 && z < DATA_RENDER_DISTANCE + 6)
                    chunk->Draw(minecraft.chunkShader);
            }
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
};

#endif