#ifndef UI_CLASS_H
#define UI_CLASS_H

#include "shader.h"
#include "VAO.h"
#include "player.h"
#include "textDisplay.h"

class UserInterface {
    glm::vec2* windowSize;
    Shader highlightShader;
    TextDisplay text;
    VAO highlight;
public:
    bool hasHighlight;


    UserInterface() {
        initHighlight();
        hasHighlight = false;
    }

    void Init(glm::vec2 *windowSize) {
        this->windowSize = windowSize;
        text.Init(windowSize->x, windowSize->y);
    }

    void InitUniforms(glm::mat4& projection) {
        highlightShader.Activate();
        highlightShader.setMat4("projection", projection);
    }

    void  setTextProjection(float width, float height) {
        text.setProjection(width, height);
    }

    void SetViewMatrix(glm::mat4& view) {
        highlightShader.Activate();
        highlightShader.setMat4("view", view);
    }

    void Draw(Player &player) {
        SetHighlight(player.selectedCube);
        DrawHighlight();
        DrawCross(windowSize->x / 2, windowSize->y / 2);
    }

private:


    void SetHighlight(RayCastInfo selectedCube) {
        hasHighlight = false;
        if (selectedCube.id == AIR || selectedCube.range < 1)
            return;
        highlightShader.Activate();
        highlightShader.setVec3("cubePos", selectedCube.position);
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

    void DrawCross(float x, float z) {
        text.display("+", x, z, 0.4f, glm::vec3(1.0, 1.0f, 1.0f));
    }

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
