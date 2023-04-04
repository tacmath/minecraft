#include "background.h"

Background::Background(void) {
    initSkybox();
    shader.Load("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
}

// destructor
Background::~Background(void) {
    shader.Delete();
}

// draw the skybox
void Background::Draw(void) {
    glDepthMask(false);
    glDepthFunc(GL_LEQUAL);
    shader.Activate();
    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    sun.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(true);
}

//load the view matrix in all the shaders
void Background::LoadViewMatrix(const Camera& camera) {
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));

    shader.setMat4("view", view);
    sun.SetView(view);
}

void Background::initUniforms(const Camera& camera) {
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));

    shader.setInt("skybox", 0);
    shader.setMat4("projection", camera.projection);
    shader.setMat4("view", view);
    sun.SetView(view);
    sun.SetProjection(camera.projection);
}


void Background::initSkybox(void) {
    std::array<std::string, 6> texturesName;

    float skyboxVertices[] =
    {
        //   Coordinates
        -1.f, 1.f, 1.f,     // Front-top-left
        1.f, 1.f, 1.f,      // Front-top-right
        -1.f, -1.f, 1.f,    // Front-bottom-left
        1.f, -1.f, 1.f,     // Front-bottom-right
        1.f, -1.f, -1.f,    // Back-bottom-right
        1.f, 1.f, 1.f,      // Front-top-right
        1.f, 1.f, -1.f,     // Back-top-right
        -1.f, 1.f, 1.f,     // Front-top-left
        -1.f, 1.f, -1.f,    // Back-top-left
        -1.f, -1.f, 1.f,    // Front-bottom-left
        -1.f, -1.f, -1.f,   // Back-bottom-left
        1.f, -1.f, -1.f,    // Back-bottom-right
        -1.f, 1.f, -1.f,    // Back-top-left
        1.f, 1.f, -1.f      // Back-top-right
    };

    texturesName[0] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_2_Left+X.png";
    texturesName[1] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_3_Right-X.png";
    texturesName[2] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_4_Up+Y.png";
    texturesName[3] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_5_Down-Y.png";
    texturesName[4] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_0_Front+Z.png";
    texturesName[5] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_1_Back-Z.png";
    

    cubemap.Gen(0).Load(texturesName);

    vao.Gen();
    vao.LinkAttrib((void*)skyboxVertices, 14, 0, 3, GL_FLOAT, sizeof(float), 0);
}