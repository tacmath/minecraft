#ifndef DEBUG_UTILS_CLASS_H
#define DEBUG_UTILS_CLASS_H

#include <glad/glad.h>
#include <iostream>

struct DebugUtils {
private:
    GLuint quadVAO;
    GLuint quadVBO;

    GLuint FBO;
    GLuint RBO;
    GLuint frameBufferTexture;
    GLuint FBWidth;
    GLuint FBHeight;
public:
    DebugUtils() {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f
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
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenFramebuffers(1, &FBO);
        glGenRenderbuffers(1, &RBO);
        glGenTextures(1, &frameBufferTexture);
    }
    
    void renderQuad()
    {
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void renderQuad(glm::vec2 coord, glm::vec2 size)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            coord.x,            coord.y + size.y,   0.0f, 0.0f, 1.0f,
            coord.x,            coord.y,            0.0f, 0.0f, 0.0f,
            coord.x + size.x,   coord.y + size.y,   0.0f, 1.0f, 1.0f,
            coord.x + size.x,   coord.y,            0.0f, 1.0f, 0.0f
        };

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initRenderFBO(int whidth, int height, GLuint textUnit) {
        FBWidth = whidth;
        FBHeight = height;
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        glActiveTexture(GL_TEXTURE0 + textUnit);
 
        glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, whidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, whidth, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bindRenderFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glViewport(0, 0, FBWidth, FBHeight);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
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

};

#endif