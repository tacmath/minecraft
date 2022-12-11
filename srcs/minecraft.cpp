#include "minecraft.h"
#include "perlinNoise.h"
#include <algorithm>

void parseBlockData(std::vector<std::string>& textures);

Minecraft::Minecraft(void) {
    window = 0;
    windowSize = glm::vec2((float)DEFAULT_WINDOW_WIDTH, (float)DEFAULT_WINDOW_HEIGHT);
    initWindows();
    initSkybox();
    enableGlParam();
    skyboxShader.Load("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
    normalChunkShader.Load("shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
    wireframeChunkShader.Load("shaders/cubeVS.glsl", "shaders/wireFrameFS.glsl", "shaders/wireFrameGS.glsl");
    chunkShader = normalChunkShader;

    std::vector<std::string> textureNames;
    parseBlockData(textureNames);
    for (auto& name : textureNames)
        name = "texture/" + name;
    texAtlas.LoadArray(textureNames, 0);
    
    initChunks(STARTING_RENDER_DISTANCE);

    loadedChunks = (Chunk**)calloc((DATA_RENDER_DISTANCE << 1) * (DATA_RENDER_DISTANCE << 1), sizeof(Chunk*));
    if (!loadedChunks)
        exit(1);
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
void Minecraft::LoadViewMatrix(Camera& camera) {
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
    free(loadedChunks);
    normalChunkShader.Delete();
    wireframeChunkShader.Delete();
    skyboxShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
}

inline bool cmpChunk(Chunk* a, const Chunk* b) {
    return ((a->playerProximity + (!a->isVisible << 10)) < (b->playerProximity + (!b->isVisible << 10)));
}

void Minecraft::fillLoadedChunks(std::vector<Chunk*>& chunks, glm::vec3& position) {
    int x, z, playerPosx, playerPosz, maxChunk;
    int n, chunkNumber;
    Chunk* chunk;

    maxChunk = DATA_RENDER_DISTANCE << 1;
    playerPosx = ((int)position.x >> 4) - DATA_RENDER_DISTANCE;
    playerPosz = ((int)position.z >> 4) - DATA_RENDER_DISTANCE;
    chunkNumber = (int)chunks.size();
    n = -1;
    while (++n < chunkNumber) {
        chunk = chunks[n];
        x = chunk->posx - playerPosx;
        z = chunk->posz - playerPosz;

        // check if a chunk is too far away and delete it if nessesary
        if ((x < -UNLOAD_OFFSET || z < -UNLOAD_OFFSET || x > maxChunk + UNLOAD_OFFSET || z > maxChunk + UNLOAD_OFFSET) && chunk->threadStatus == CHUNK_NOT_PROCESSING) {
            delete chunk; // delete (chunk destructor) = 60 % of the function perf
            chunks.erase(chunks.begin() + n);
            n--;
            chunkNumber--;
        }
        // fill the 2d array if a chunk is in the RENDER_DISTANCE
        else if (x >= 0 && z >= 0 && x < maxChunk && z < maxChunk)
            loadedChunks[x * maxChunk + z] = chunk;
    }
}

void Minecraft::sortChunksLoading(glm::vec3& position, Camera& camera) {
    for (int n = 0; n < chunksLoading.size(); n++) {
        Chunk* chunk;

        chunk = chunksLoading[n];
        chunk->SetPlayerProximity(position); //sqrt take some time
        chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz, 24);
    }
    std::sort(chunksLoading.begin(), chunksLoading.end(), cmpChunk); //instead sort from middle to borders and maybe limit the ammount of chunks in chunksLoading
}



void Minecraft::loadNewChunks(glm::vec3 &position) {
    int playerPosx, playerPosz, maxChunk;

    maxChunk = DATA_RENDER_DISTANCE << 1;
    playerPosx = ((int)position.x >> 4) - DATA_RENDER_DISTANCE;
    playerPosz = ((int)position.z >> 4) - DATA_RENDER_DISTANCE;
    //add all the chunk that are in loadedChunks but didn't exist
    for (int x = 0; x < maxChunk; x++)
        for (int z = 0; z < maxChunk; z++) {
            // load new chunks if it is in the render distance but didn't exist
            if (!loadedChunks[x * maxChunk + z] /*&& VEC2_LEN((x - DATA_RENDER_DISTANCE), (z - DATA_RENDER_DISTANCE)) <= DATA_RENDER_DISTANCE*/) {
                Chunk* newChunk = new Chunk;
                newChunk->SetPosistion(playerPosx + x, playerPosz + z);
                chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
                chunksLoading.push_back(newChunk);
                loadedChunks[x * maxChunk + z] = newChunk;
            }
        }
}

void Minecraft::LoadChunks(glm::vec3 &position, Camera& camera) {

    memset(loadedChunks, 0, (DATA_RENDER_DISTANCE << 1) * (DATA_RENDER_DISTANCE << 1) * sizeof(Chunk*));

    fillLoadedChunks(chunks, position);
    fillLoadedChunks(chunksLoading, position);

    loadNewChunks(position);

    sortChunksLoading(position, camera);

    thread.LoadChunk(chunksLoading);
    thread.CreateMesh(chunks, chunksLoading);
}

void Minecraft::setChunksVisibility(Camera &camera) {
    Chunk* chunk;

    for (unsigned n = 0; n < chunks.size(); n++) {
        chunk = chunks[n];
        if (chunk->status == CHUNK_LOADED)
            chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz);
    }
}
