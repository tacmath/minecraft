#ifndef SHADOW_CLASS_H
#define SHADOW_CLASS_H

#include <glad/glad.h>
#include "shader.h"
#include "camera.h"
#include "world_area.h"

#define SHADOW_TEXTURE_SIZE 2048

#define SHADOW_CASCADE_NB 3

#define SHADOW_ON 1

#define SHADOW_OFF 0

// for transparent object with shadow map https://wickedengine.net/2018/01/18/easy-transparent-shadow-maps/

class Shadow {
private:
    int status;
	GLuint textureID;
    GLuint frameBufferID;
    Shader shadowShader;
    glm::mat4 biasMatrix;
    glm::mat4 projection[SHADOW_CASCADE_NB];
    glm::mat4 view[SHADOW_CASCADE_NB];
    GLFWwindow* window;
    Camera *playerCam;
    WorldArea* worldArea;

public:
    Shadow() {
        textureID = 0;
        frameBufferID = 0;
        status = SHADOW_OFF;
    }

    void Link(GLFWwindow* window, Camera* playerCam, WorldArea* worldArea) {
        this->window = window;
        this->playerCam = playerCam;
        this->worldArea = worldArea;
    }

    void Activate() {
        status = SHADOW_ON;
        glGenTextures(1, &textureID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT,
             SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE, SHADOW_CASCADE_NB, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        biasMatrix = {
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        };

        shadowShader.Load("shaders/shadowVS.glsl", "shaders/shadowFS.glsl", "shaders/shadowGS.glsl");
    }

    void Delete() {
        status = SHADOW_OFF;
        glDeleteFramebuffers(1, &frameBufferID);
        glDeleteTextures(1, &textureID);
        shadowShader.Delete();
    }

    void GenerateShadowMap(glm::vec3 lightDir) {
        if (status == SHADOW_OFF)
            return ;
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glViewport(0, 0, SHADOW_TEXTURE_SIZE, SHADOW_TEXTURE_SIZE);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        setLightViewProjectionMatrix(lightDir);

        shadowShader.Activate();
        renderChunks();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);    // reset the viewport
        glViewport(0, 0, width, height);
    }

    ~Shadow() {
        Delete();
    }


private:

    void getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view, glm::vec4 *frustumCorners) {
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
                    frustumCorners[n] = pt / pt.w;
                    n++;
                }
            }
        }
    }

    void createLightViewProjectionMatrices(glm::vec3& lightDir) {
        glm::mat4 perspectives[SHADOW_CASCADE_NB];
        glm::vec4 frustumCorners[8];

        perspectives[0] = glm::perspective(glm::radians(playerCam->GetFOV()), playerCam->GetScreenRatio(), -1.0f, 16.0f);
        perspectives[1] = glm::perspective(glm::radians(playerCam->GetFOV()), playerCam->GetScreenRatio(), 16.0f, 48.0f);
        perspectives[2] = glm::perspective(glm::radians(playerCam->GetFOV()), playerCam->GetScreenRatio(), 48.0f, 160.0f);


        for (int cascade = 0; cascade < SHADOW_CASCADE_NB; cascade++) {
            getFrustumCornersWorldSpace(perspectives[cascade], playerCam->view, frustumCorners);

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();

            glm::vec3 center = glm::vec3(0.0f);     //change to fix the center to fix the center y to the coords 60
            for (int n = 0; n < 8; n++)
                center += glm::vec3(frustumCorners[n]);
            center /= 8;

            view[cascade] = glm::lookAt(
                center + lightDir,
                center,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );

            for (int n = 0; n < 8; n++)
            {
                glm::vec4 trf = view[cascade] * frustumCorners[n];
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            projection[cascade] = glm::ortho(minX, maxX, minY, maxY, minZ - 128, maxZ + 128);
        }
    }

    void setLightViewProjectionMatrix(glm::vec3 &lightDir) {
        Shader& chunkShader = worldArea->GetShader();
        glm::mat4 lightSpaceMatrices[SHADOW_CASCADE_NB];

        createLightViewProjectionMatrices(lightDir);

        for (int n = 0; n < SHADOW_CASCADE_NB; n++)
            lightSpaceMatrices[n] = projection[n] * view[n];

        shadowShader.setMat4("lightSpaceMatrices", SHADOW_CASCADE_NB, lightSpaceMatrices);

        for (int n = 0; n < SHADOW_CASCADE_NB; n++)
            lightSpaceMatrices[n] = biasMatrix * lightSpaceMatrices[n];

        chunkShader.setMat4("lightSpaceMatrices", SHADOW_CASCADE_NB, lightSpaceMatrices);
    }

    void renderChunks() {
        std::vector<Chunk*>& chunks = worldArea->GetChunks();
        Chunk* chunk;

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(3.0f, 1.0f);
        for (size_t n = 0; n < chunks.size(); n++) {
            chunk = chunks[n];
            if (playerCam->frustum.chunkIsVisible(chunk->posx, chunk->posz, 48))
                chunk->Draw(shadowShader);
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
};

#endif