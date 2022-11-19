#include "minecraft.h"
#include "blocks.h"
#include <fstream>
#include <algorithm>

extern Block blocks[256];

static void split(std::string &str, std::vector<std::string> &result) {
    int n;
    int offset;
    int size;

    result.clear();
    n = 0;
    size = (int)str.size();
    while (n < size) {
        while ((str[n] == ' ' || str[n] == '\t') && n < size)
            n++;
        offset = n;
        while (str[n] != ' ' && str[n] != '\t' && n < size)
            n++;
        if (offset < n)
            result.push_back(str.substr(offset, n - offset));
    }
}

static void parseBlockData(std::vector<std::string> &textures) {
    std::ifstream   infile("cubes_settings");
    std::vector<std::string> splitLine;
    Block           block;
    unsigned char   blockID;

    block.SetTextures(0, 0, 0);
    blockID = 0;
    
    for (std::string line; std::getline(infile, line); ) {
        if (!line.size())
            continue;
        if (line.substr(0, 4) == "Name") {
            blocks[blockID] = block;
            blockID = 0;
            block.SetTextures(0, 0, 0);
        }
        else if (line.substr(1, 2) == "ID")
            blockID = std::stoi(line.substr(3, line.size() - 3));
        else {
            split(line, splitLine);
            if (splitLine.size() < 2)
                continue;

            if (splitLine[0] == "Top" || splitLine[0] == "Side" || splitLine[0] == "Bottom") {
                auto it = std::find(textures.begin(), textures.end(), splitLine[1]);
                char index;
                
                if (it != textures.end())
                    index = it - textures.begin();
                else {
                    index = (char)textures.size();
                    textures.push_back(splitLine[1]);
                }
                block.SetTexture(splitLine[0], index);
            }
        }
    }
    blocks[blockID] = block;
}

Minecraft::Minecraft(void) {
    window = 0;
    initWindows();
    initSkybox();
    enableGlParam();
    skyboxShader.Load("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
    chunkShader.Load("shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
    camera.Init((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, glm::vec3(0.0f, 60.0f, 0.0f));
    
    std::vector<std::string> textureNames;
    parseBlockData(textureNames);
    for (auto& name : textureNames)
        name = "texture/" + name;
    texAtlas.LoadAtlas(textureNames, 0);
    
    initUniforms();

    seed = (int)((double)rand() / (RAND_MAX)) * UINT32_MAX;
    global_noise.SetSeed(seed);

    initChunks(STARTING_RENDER_DISTANCE);

}

void Minecraft::Draw(void) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    skyboxShader.Activate();
    skybox.Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);


    glEnable(GL_CULL_FACE);
    chunkShader.Activate();
    for (int n = 0; n < chunks.size(); n++)
        chunks[n]->Draw(chunkShader);
    glDisable(GL_CULL_FACE);
}


//load the view matrix in all the shaders
void Minecraft::LoadViewMatrix(void) {
    chunkShader.Activate();
    chunkShader.setMat4("view", camera.view);

    skyboxShader.Activate();
    skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.view)));
}

//destructor
Minecraft::~Minecraft(void) {
    //std::cout << "Minecraft destructor has been called" << std::endl;
    for (int n = 0; n < chunks.size(); n++)
        delete chunks[n];
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Minecraft::LoadChunks(void) {
    Chunk    *loadedChunks[DATA_RENDER_DISTANCE << 1][DATA_RENDER_DISTANCE << 1];     //probably need to change that for a dynamic table
    int x, z, playerPosx, playerPosz, maxChunk;
    int n, chunkNumber;
    maxChunk = DATA_RENDER_DISTANCE << 1;
    playerPosx = ((int)camera.position.x >> 4) - DATA_RENDER_DISTANCE;
    playerPosz = ((int)camera.position.z >> 4) - DATA_RENDER_DISTANCE;
    memset(loadedChunks, 0, maxChunk * maxChunk * sizeof(Chunk*));
    chunkNumber = (int)chunks.size();
    n = -1;
    while (++n < chunkNumber) {
        x = chunks[n]->posx - playerPosx;
        z = chunks[n]->posz - playerPosz;

        // check if a chunk is too far away and delete it if nessesary
        if ((x < -UNLOAD_OFFSET || z < -UNLOAD_OFFSET || x > maxChunk + UNLOAD_OFFSET || z > maxChunk + UNLOAD_OFFSET) && chunks[n]->threadStatus == CHUNK_NOT_PROCESSING) {
            delete chunks[n]; // delete (chunk destructor) = 60 % of the function perf
            chunks.erase(chunks.begin() + n);
            n--;
            chunkNumber--;
        }
        // fill the 2d array if a chunk is in the RENDER_DISTANCE
        else if (x >= 0 && z >= 0 && x < maxChunk && z < maxChunk)
            loadedChunks[x][z] = chunks[n];
    }

    //add all the chunk that are in loadedChunks but didn't exist
    for (int x = 0; x < maxChunk; x++)
        for (int z = 0; z < maxChunk; z++) {

            // load new chunks if it is in the render distance but didn't exist
            if (!loadedChunks[x][z]/* && camera.frustum.chunkIsVisible(playerPosx + x, playerPosz + z)*/) {    //change the order of generation to start from the middle to the side
                Chunk* newChunk = new Chunk;     //push_back is creating a copy
                newChunk->SetPosistion(playerPosx + x, playerPosz + z);
                chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
                thread.LoadChunk(newChunk);
                chunks.push_back(newChunk);     //if needed push_back fist the most important chunk or create a priority list
                loadedChunks[x][z] = newChunk;
                continue;
            }
            if (!loadedChunks[x][z] || loadedChunks[x][z]->threadStatus & CHUNK_PROCESSING)
                continue;

            // add chunks to thread that where not able to be processed
            if (loadedChunks[x][z]->status == CHUNK_UNLOADED)
                thread.LoadChunk(loadedChunks[x][z]);
            else if (loadedChunks[x][z]->status == CHUNK_DATA_LOADED)
                thread.CreateMesh(loadedChunks[x][z]);
        }
}

void Minecraft::setChunksVisibility(void) {
    Chunk* chunk;

    for (unsigned n = 0; n < chunks.size(); n++) {
        chunk = chunks[n];
        if (chunk->status == CHUNK_LOADED)
            chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz);
    }
}