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

#include "stb_image.h"
#include "shader.h"
#include "VAO.h"
#include "EBO.h"
#include "texture.h"
#include "camera.h"
#include "cubeMap.h"
#include "chunk.h"
#include "thread.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

# define   PI           3.14159265358979323846
# define WINDOW_WIDTH   1200
# define WINDOW_HEIGHT  900


# define STARTING_RENDER_DISTANCE 1
# define RENDER_DISTANCE 10
# define UNLOAD_OFFSET 2

class Minecraft {
public:
    // the window context
    GLFWwindow* window;

    // the camera
    Camera      camera;

    // a chunk
	std::vector<Chunk*> chunks;

    // multithreading object
    Thread thread;
    
    //chunk shader
    Shader  chunkShader;
    // texture atlas
    Texture texAtlas;

    // skybox vexter array object
	VAO         skybox;
    // skybox shader
    Shader  skyboxShader;
    // skybox cubemap
    CubeMap skyboxCubemap;
    
    // seed used to generate random chunk
    unsigned int seed;



	Minecraft(void);

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
        for (int n = 0; n < chunks.size(); n++)
       //     if (camera.frustum.isVisible((float)(chunks[n]->posx << 4), (float)(chunks[n]->posz << 4), CHUNK_SIZE))
                chunks[n]->Draw(chunkShader);
        glDisable(GL_CULL_FACE);
    }


    //load the view matrix in all the shaders
    void LoadViewMatrix(void) {
        chunkShader.Activate();
        chunkShader.setMat4("view", camera.view);

        skyboxShader.Activate();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.view)));
    }

    //destructor
    ~Minecraft(void) {
        //std::cout << "Minecraft destructor has been called" << std::endl;
        for (int n = 0; n < chunks.size(); n++)
            delete chunks[n];
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void LoadChunks();

private:
    void initWindows(void);
    void initSkybox(void);
    void initChunks(int radius);
    void initUniforms(void);
    void enableGlParam(void);
};

#endif