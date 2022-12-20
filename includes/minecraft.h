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
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


# define PI             3.14159265358979323846

# define STARTING_RENDER_DISTANCE 1
# define RENDER_DISTANCE 20
# define DATA_RENDER_DISTANCE (RENDER_DISTANCE + 1)
# define UNLOAD_OFFSET 2

class Minecraft {
private:
    //list of all the chunks in the render distance
    Chunk** loadedChunks;
public:

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
    void LoadViewMatrix(Camera& camera);

    // create and delete chunks based on the render distance
    void LoadChunks(glm::vec3& position, Camera& camera);

    // set the visibility of each chunk
    void setChunksVisibility(Camera& camera);

    void initUniforms(Camera& camera);

private:
    void initSkybox(void);
    void initChunks(int radius);
    void enableGlParam(void);
    void fillLoadedChunks(std::vector<Chunk*>& chunks, glm::vec3 &position);
    void sortChunksLoading(glm::vec3& position, Camera &camera);
    void loadNewChunks(glm::vec3& position);
};

#endif