#ifndef WORLD_AREA_H
# define WORLD_AREA_H

#include "stb_image.h"
#include "shader.h"
#include "VAO.h"
#include "texture.h"
#include "camera.h"
#include "chunk.h"
#include "thread.h"
#include "parseConfig.h"
#include "soundBuffer.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


# define PI             3.14159265358979323846

# define STARTING_RENDER_DISTANCE 1
# define RENDER_DISTANCE 20
# define DATA_LOAD_DISTANCE(renderDistance) (renderDistance) + 1
# define UNLOAD_OFFSET 2

class WorldArea {
private:
    // list of all the chunks loaded
	std::vector<Chunk*> chunks;

    // list of all the chunks loading
    std::vector<Chunk*> chunksLoading;

    //list of all the chunks in the render distance
    Chunk** loadedChunks; //maybe use unique_ptr

    //LOAD_DISTANCE of the data
    unsigned dataLoadDistance;

    //chunk shader
    Shader  chunkShader;  //maybe set as static
    // texture atlas
    Texture texAtlas;

public:

    // multithreading object
    ThreadControleur thread;

    // constuctor
	WorldArea(void);
    // destructor
    ~WorldArea(void);

    // initialize the texure atlas, uniform and chunks
    void Init(const Camera& camera, Sound& sound);

    // draw the chunks and the skybox
    void Draw(void);

    //load the view matrix in all the shaders
    void LoadViewMatrix(const Camera& camera);

    // create and delete chunks based on the render distance
    void LoadChunks(const glm::vec3& position, const Camera& camera);

    // set the visibility of each chunk
    void setChunksVisibility(const Camera& camera);

    // change the render distance 
    void UpdateRenderDistance(unsigned newRenderDistance);

    // use options to reload the chunk shader
    void ReloadShader(bool wireframeMode, std::vector<std::string> shaderOption);

    void initUniforms(const Camera& camera);

    std::vector<Chunk*>& GetChunks();

    Shader& GetShader();

private:
    void initChunks(unsigned radius);
    void fillLoadedChunks(std::vector<Chunk*>& chunks, const glm::vec3 &position);
    void sortChunksLoading(const glm::vec3& position, const Camera &camera);
    void loadNewChunks(const glm::vec3& position);
};

#endif