#include "background.h"

Background::Background(void) {
    initSkybox();
    shader.Load("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
}

// destructor
Background::~Background(void) {
    shader.Delete();
    skyboxEBO.Delete();
}

// draw the skybox
void Background::Draw(void) {
    glDepthMask(false);
    glDepthFunc(GL_LEQUAL);
    shader.Activate();
    VAO.Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    sun.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(true);
}

//load the view matrix in all the shaders
void Background::LoadViewMatrix(Camera& camera) {
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));

    shader.Activate();
    shader.setMat4("view", view);
    sun.SetView(view);
}

void Background::initUniforms(Camera& camera) {
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));

    shader.Activate();
    shader.setInt("skybox", 0);
    shader.setMat4("projection", camera.projection);
    shader.setMat4("view", view);
    sun.SetView(view);
    sun.SetProjection(camera.projection);
}


void Background::initSkybox(void) {
    const char** texturesName;

    float skyboxVertices[] =
    {
        //   Coordinates
        -1.0f, -1.0f,  1.0f,//        7--------6
        1.0f, -1.0f,  1.0f,//       /|       /|
        1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
        1.0f,  1.0f,  1.0f,//      |/       |/
        1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };

    float texturesUv[] =
    {
        //   Coordinates
        0.0f,0.0f,
        0.0f,1.0f,
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        1.0f,0.0f,
        1.0f,1.0f
    };

    unsigned int skyboxIndices[] =
    {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };

    std::vector <GLuint> indices(skyboxIndices, skyboxIndices + sizeof(skyboxIndices) / sizeof(GLuint));

    texturesName = new const char* [6];
    texturesName[0] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_2_Left+X.png";
    texturesName[1] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_3_Right-X.png";
    texturesName[2] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_4_Up+Y.png";
    texturesName[3] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_5_Down-Y.png";
    texturesName[4] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_0_Front+Z.png";
    texturesName[5] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_1_Back-Z.png";
    

    skyboxEBO.Init(indices);
    cubemap.Gen(0);
    cubemap.Load(texturesName);
    delete[] texturesName;

    VAO.Gen();
    VAO.LinkAttrib((void*)skyboxVertices, 8, 0, 3, GL_FLOAT, sizeof(float), (void*)0);
    VAO.LinkAttrib((void*)texturesUv, 8, 1, 2, GL_FLOAT, sizeof(float), (void*)0);
    VAO.Bind();
    skyboxEBO.Bind();
}