#ifndef UI_CLASS_H
#define UI_CLASS_H

#include "shader.h"
#include "VAO.h"
#include "player.h"
#include "quad.h"
#include "texture.h"

class UserInterface {
    Shader  highlightShader;
    Quad    quad;
    Texture cursorTex;
    Shader  quadShader;
    VAO highlight;

public:
    bool hasHighlight;

    UserInterface() {
        initHighlight();
        quad.Init(glm::vec2(-0.015f), glm::vec2(0.03f));
        quadShader.Load("shaders/quadVS.glsl", "shaders/quadFS.glsl");
        quadShader.setInt("Texture", 1);
        cursorTex.Load("texture/ui/cursor.png", 1);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        hasHighlight = false;
    }

    void InitUniforms(glm::mat4& projection) {
        highlightShader.setMat4("projection", projection);
        float scale = projection[1][1] / projection[0][0];
        glm::mat4 ortho = glm::ortho(-1.0f * scale, 1.0f * scale, -1.0f, 1.0f);
        quadShader.setMat4("MVP", ortho);
    }

    void SetViewMatrix(glm::mat4& view) {
        highlightShader.setMat4("view", view);
    }

    void Draw(Player &player) {
        SetHighlight(player.selectedCube);
        DrawHighlight();
        DrawCross();
    }

private:


    void SetHighlight(RayCastInfo selectedCube) {
        hasHighlight = false;
        if (selectedCube.id == AIR || selectedCube.range == 0)
            return;
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

    void DrawCross() {
        glEnable(GL_BLEND);
        quadShader.Activate();
        quad.Render();
        glDisable(GL_BLEND);
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
        highlight.LinkAttrib((void*)CubeVertices, 24, 0, 3, GL_FLOAT, sizeof(float), 0);
    }


};


#endif
