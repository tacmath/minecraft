#ifndef UI_CLASS_H
#define UI_CLASS_H

#include "shader.h"
#include "VAO.h"

class UserInterface {
	Shader highlightShader;
	VAO highlight;
public:
    bool hasHighlight;


	UserInterface() {
        initHighlight();
        hasHighlight = false;
	}

    void InitUniforms(glm::mat4& projection) {
        highlightShader.Activate();
        highlightShader.setMat4("projection", projection);
    }

    void SetViewMatrix(glm::mat4& view) {
        highlightShader.Activate();
        highlightShader.setMat4("view", view);
    }

    void SetHighlight(glm::vec3 pos) {
        hasHighlight = false;
        if (pos.y < 0)
            return;
        highlightShader.Activate();
        highlightShader.setVec3("cubePos", pos);
        hasHighlight = true;
    }

    void DrawHighlight() {
        if (!hasHighlight)
            return;
//        glEnable(GL_MULTISAMPLE);
        highlightShader.Activate();
        highlight.Bind();
        glDrawArrays(GL_LINES, 0, 24);
    }

private:
	void initHighlight() {

        float CubeVertices[] =
        {
            //   Coordinates
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,

            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,

            0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,

            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f,

            1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };

		highlightShader.Load("shaders/highlightVS.glsl", "shaders/highlightFS.glsl");
        
        highlight.Gen();
        highlight.LinkAttrib((void*)CubeVertices, 24, 0, 3, GL_FLOAT, sizeof(float), (void*)0);
	}


};


#endif
