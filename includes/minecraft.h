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
#include "motor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "simplex_noise.h"


# define PI             3.14159265358979323846
# define DEFAULT_WINDOW_WIDTH  1920
# define DEFAULT_WINDOW_HEIGHT  1080
# define MAX_FPS        4000


# define STARTING_RENDER_DISTANCE 1
# define RENDER_DISTANCE 40
# define DATA_RENDER_DISTANCE (RENDER_DISTANCE + 1)
# define UNLOAD_OFFSET 2

class Minecraft {
private:
    //list of all the chunks in the render distance
    Chunk** loadedChunks;
public:
    // the window context
    GLFWwindow* window;

    // the camera
    Camera      camera;

    // the player
    Player      player;

    Motor       motor;

    glm::vec2 windowsSize;

    // list of all the chunks loaded
	std::vector<Chunk*> chunks;

    // list of all the chunks loading
    std::vector<Chunk*> chunksLoading;

    // multithreading object
    ThreadControleur thread;
    
    //normal chunk shader
    Shader  normalChunkShader;
    //wireframe chunk shader
    Shader  wireframeChunkShader;
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

    void changeShader(Shader &currentShader, Shader &newShader);

private:
    void initWindows(void);
    void initSkybox(void);
    void initChunks(int radius);
    void initUniforms(void);
    void enableGlParam(void);
    void fillLoadedChunks(std::vector<Chunk*>& chunks);
    void sortChunksLoading(void);
    void loadNewChunks(void);
};

#endif