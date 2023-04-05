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

#define INVENTORY_BARRE_X 184
#define INVENTORY_BARRE_Y 24
#define IB_SLOT_SIZE 20

class InventoryBarre {
    Texture texture;
    Texture selectTexture;
    Quad    inventoryQuad;
    Shader  quadShader;
    Shader  quadShaderArray;
    GLuint  inventoryTextureID;
    GLuint  frameBufferID;

public:
    InventoryBarre() {
        inventoryTextureID = 0;
        glCreateFramebuffers(1, &frameBufferID);
    }

    ~InventoryBarre() {
        glDeleteTextures(1, &inventoryTextureID);
        glDeleteFramebuffers(1, &frameBufferID);
        quadShader.Delete();
    }

    void Init() {
        quadShader.Load("shaders/quadVS.glsl", "shaders/quadFS.glsl");
        std::vector<std::string> options = { "ARRAY" };
        quadShaderArray.Load(options, "shaders/quadVS.glsl", "shaders/quadFS.glsl");
        quadShader.setInt("Texture", 0);
        quadShaderArray.setInt("Texture", 0);
        texture.Load("texture/ui/menu_bar.png", 0);
        selectTexture.Load("texture/ui/selector.png", 0);
        inventoryQuad.Init(glm::vec2(-0.7f, -0.95f), glm::vec2(1.4f, 1.4f * 24.0f / 184.0f));
        
        glCreateTextures(GL_TEXTURE_2D, 1, &inventoryTextureID);
        glBindTextureUnit(2, inventoryTextureID);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTextureParameteri(inventoryTextureID, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTextureStorage2D(inventoryTextureID, 1, GL_RGBA8, INVENTORY_BARRE_X * 4, INVENTORY_BARRE_Y * 4);
       
        glNamedFramebufferTexture(frameBufferID, GL_COLOR_ATTACHMENT0, inventoryTextureID, 0);
    }

    void Update(unsigned cursorPos, const inventory_t &inventory) {
        Quad    quad;
        glm::mat4 P;
        glm::mat4 S;

        P = glm::ortho(0.0f, (float)INVENTORY_BARRE_X, (float)INVENTORY_BARRE_Y, 0.0f);
        quad.Init(glm::vec2(0), glm::vec2(1), GL_STREAM_DRAW);
        
        glEnable(GL_BLEND);
        quadShader.Activate();
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glViewport(0, 0, INVENTORY_BARRE_X * 4, INVENTORY_BARRE_Y * 4);
        glClear(GL_COLOR_BUFFER_BIT);
        texture.Bind();
        quadShader.setMat4("MVP", P * glm::scale(glm::translate(glm::vec3(1.0f, 1.0f, 0.0f)), glm::vec3(INVENTORY_BARRE_X - 2.0f, INVENTORY_BARRE_Y - 2.0f, 0.0f)));
        quad.Render();

        selectTexture.Bind();
        quadShader.setMat4("MVP", P * glm::translate(glm::vec3(IB_SLOT_SIZE * cursorPos, 0.0f, 0.0f)) * glm::scale(glm::vec3(INVENTORY_BARRE_Y, INVENTORY_BARRE_Y, 0.0f)));
        quad.Render();

        S = glm::scale(glm::vec3(14, 14, 0.0f));
        quadShaderArray.Activate();
        for (unsigned n = 0; n < INVENTORY_SIZE; n++) {
            if (inventory[n]) {
                quadShaderArray.setInt("texIndex",Chunk::blocks[inventory[n]].top);
                quadShaderArray.setMat4("MVP", P * glm::translate(glm::vec3(IB_SLOT_SIZE * n + 5.0f, 5.0f, 0.0f)) * S);
                quad.Render();
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_BLEND);
    }

    void Draw() {
        inventoryQuad.Render();
    }
};

class UserInterface {
    Shader  highlightShader;
    Quad    quad;
    Texture cursorTex;
    Shader  quadShader;
    InventoryBarre inventoryBarre;
    VAO     highlight;
    

public:
    bool hasHighlight;

    UserInterface() {
        initHighlight();
        InitUI();
    }

    ~UserInterface() {
        highlightShader.Delete();
        quadShader.Delete();
    }

    void InitUniforms(const glm::mat4& projection) {
        highlightShader.setMat4("projection", projection);
        float scale = projection[1][1] / projection[0][0];
        glm::mat4 ortho = glm::ortho(-1.0f * scale, 1.0f * scale, -1.0f, 1.0f);
        quadShader.setMat4("MVP", ortho);
    }

    void SetViewMatrix(const glm::mat4& view) {
        highlightShader.setMat4("view", view);
    }

    void UpdateInventory(const Player& player) {
        inventoryBarre.Update(player.GetSelectedSlot(), player.GetInventory());
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
        inventoryBarre.Init();
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
        inventoryBarre.Draw();
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
