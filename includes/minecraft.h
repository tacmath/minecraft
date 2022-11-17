#ifndef MINECRAFT_H
# define MINECRAFT_H

#include "stb_image.h"
#include "shader.h"
#include "VAO.h"
#include "texture.h"
#include "camera.h"
#include "cubeMap.h"
#include "chunk.h"
#include "thread.h"
#include "player.h"
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
# define DATA_RENDER_DISTANCE (RENDER_DISTANCE + 1)
# define UNLOAD_OFFSET 2

class Minecraft {
public:
    // the window context
    GLFWwindow* window;

    // the camera
    Camera      camera;

    // the player
    Player      player;

    // a chunk
	std::vector<Chunk*> chunks;

    // multithreading object
    ThreadControleur thread;
    
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


    // constuctor
	Minecraft(void);
    // destructor
    ~Minecraft(void);

    // draw the chunks and the skybox
    void Draw(void);

    //load the view matrix in all the shaders
    void LoadViewMatrix(void);

    // create and delete chunks based on the render distance
    void LoadChunks(void);

    // set the visibility of each chunk
    void setChunksVisibility(void);

private:
    void initWindows(void);
    void initSkybox(void);
    void initChunks(int radius);
    void initUniforms(void);
    void enableGlParam(void);
};

#endif