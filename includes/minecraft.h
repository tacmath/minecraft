#ifndef MINECRAFT_H
# define MINECRAFT_H

#ifdef _WIN32
#include <direct.h>
// MSDN recommends against using getcwd & chdir names
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "VAO.h"
#include "EBO.h"
#include "texture.h"
#include "camera.h"
#include "cubeMap.h"
#include "chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

# define   PI           3.14159265358979323846
# define WINDOW_WIDTH   1200
# define WINDOW_HEIGHT  900

class Minecraft {
public:
    // the window context
    GLFWwindow* window;

    // the camera
    Camera      camera;

    // a chunk
	Chunk       chunk;
    //chunk shader
    Shader  chunkShader;

    // skybox vexter array object
	VAO         skybox;
    // skybox shader
    Shader  skyboxShader;
    // skybox cubemap
    CubeMap skyboxCubemap;
    
    // seed used to generate random chunk
    unsigned int seed;

	Minecraft() {
        window = 0;
        initWindows();
        initSkybox();
        skyboxShader.Load("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
        chunkShader.Load("shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
        camera.Init((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, glm::vec3(0.0f, 10.0f, 0.0f));

        enableGlParam();
        initUniforms();

        std::srand(std::time(nullptr));
        seed = (unsigned int)std::rand();
        chunk.Generate(seed);
	}

    // draw the chunks and the skybox
    void Draw(void) {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.Activate();
        skybox.Bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthFunc(GL_LESS);


        glEnable(GL_CULL_FACE);
        chunkShader.Activate();
        chunk.Draw();
        glDisable(GL_CULL_FACE);
    }

    //load the view matrix in all the shaders
    void LoadViewMatrix(void) {
        chunkShader.Activate();
        chunkShader.setMat4("view", camera.view);

        skyboxShader.Activate();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.view)));
    }

private:
    void initWindows(void);
    void initSkybox(void);
    void initUniforms(void);
    void enableGlParam(void);
};

void Minecraft::initWindows(void) {
    char path[1024];

    cwd(path, 1024);
    if (!glfwInit()) {
        printf("Glfw failed to init\n");
        exit(-1);
    }
    cd(path);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minecraft", NULL, NULL))) {
        glfwTerminate();
        printf("Glfw failed to create a window\n");
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void Minecraft::initSkybox(void) {
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
    EBO<GLuint> skyboxEBO(indices);

    texturesName = new const char* [6];
    texturesName[0] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_2_Left+X.png";
    texturesName[1] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_3_Right-X.png";
    texturesName[2] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_4_Up+Y.png";
    texturesName[3] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_5_Down-Y.png";
    texturesName[4] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_0_Front+Z.png";
    texturesName[5] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_1_Back-Z.png";
    CubeMap skyboxtex(texturesName, 0);

    skyboxCubemap.ID = skyboxtex.ID;
    skyboxCubemap.unit = skyboxtex.unit;

    skybox.Gen();
    
    skybox.LinkAttrib((void*)skyboxVertices, 8, 0, 3, GL_FLOAT, sizeof(float), (void*)0);
    skybox.LinkAttrib((void*)texturesUv, 8, 1, 2, GL_FLOAT, sizeof(float), (void*)0);
    skybox.Bind();
    skyboxEBO.Bind();
    skyboxCubemap.Bind();
}

void Minecraft::initUniforms(void) {
    chunkShader.Activate();
    chunkShader.setInt("dirt", 0);
    chunkShader.setMat4("projection", camera.projection);
    chunkShader.setMat4("view", camera.view);

    skyboxShader.Activate();
    skyboxShader.setInt("skybox", 0);
    skyboxShader.setMat4("projection", camera.projection);
    skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.view)));
}

void Minecraft::enableGlParam(void) {
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

#endif