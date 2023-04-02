#ifndef UI_CLASS_H
#define UI_CLASS_H

#include "shader.h"
#include "VAO.h"
#include "player.h"
#include "quad.h"
#include "texture.h"

/*
class Sprite {
    GLuint      vao;
    GLuint      vbo;
public:
    glm::ivec2  size;

    void Load(const char *fileName);
  //  void LoadFromAtlas(glm::ivec2 start, glm::ivec2 end, Atlas atlas);
    void Render();
};*/

class UserInterface {
    Shader  highlightShader;
    Quad    quad;
    Texture cursorTex;
    Texture inventoryTex;
    Shader  quadShader;
    Quad    inventory;
    VAO     highlight;

    GLuint  inventoryTextureID;

public:
    bool hasHighlight;

    UserInterface() {
        inventoryTextureID = 0;
        initHighlight();
        InitUI();
    }

    ~UserInterface() {
        glDeleteTextures(1, &inventoryTextureID);
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

    void UpdateInventory() {
    }

    void Draw(Player &player) {
        SetHighlight(player.selectedCube);
        DrawHighlight();
        DrawUI();
    }

private:

    void InitUI() {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        quad.Init(glm::vec2(-0.015f), glm::vec2(0.03f));
        quadShader.Load("shaders/quadVS.glsl", "shaders/quadFS.glsl");
        cursorTex.Load("texture/ui/cursor.png", 1);
        inventoryTex.Load("texture/ui/menu_bar.png", 2);
        inventory.Init(glm::vec2(-0.7f, -0.95f), glm::vec2(1.4f, 0.15f));
        
        /*
        glCreateTextures(GL_TEXTURE_2D, 1, &inventoryTextureID);
        glBindTextureUnit(GL_TEXTURE2, inventoryTextureID);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTextureStorage2D(inventoryTextureID, 1, GL_RGBA, 182, 22);*/

        
    }


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

    void DrawUI() {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        quadShader.Activate();
        quadShader.setInt("Texture", 1);
        quad.Render();
        quadShader.setInt("Texture", 2);
        inventory.Render();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
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
        hasHighlight = false;
    }


};


#endif
