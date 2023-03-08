#include "world_area.h"
#include "perlinNoise.h"
#include <algorithm>

void parseBlockData(std::vector<std::string>& textures);

WorldArea::WorldArea(void) {
    std::vector<std::string> shaderOption;
    shaderOption.push_back("SHADOW");

    chunkShader.Load(shaderOption, "shaders/cubeVS.glsl", "shaders/cubeFS.glsl");

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

void WorldArea::Draw(void) {
    glEnable(GL_CULL_FACE);
    chunkShader.Activate();
    for (size_t n = 0; n < chunks.size(); n++)
        chunks[n]->DrawVisible(chunkShader);
    glDisable(GL_CULL_FACE);
}


//load the view matrix in all the shaders
void WorldArea::LoadViewMatrix(Camera& camera) {
    chunkShader.setMat4("view", camera.view);
}

//destructor
WorldArea::~WorldArea(void) {
    //std::cout << "WorldArea destructor has been called" << std::endl;
    for (size_t n = 0; n < chunks.size(); n++)
        delete chunks[n];
    for (size_t n = 0; n < chunksLoading.size(); n++)
        delete chunksLoading[n];
    free(loadedChunks);
    chunkShader.Delete();
}

void WorldArea::fillLoadedChunks(std::vector<Chunk*>& chunks, glm::vec3& position) {
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

void WorldArea::sortChunksLoading(glm::vec3& position, Camera& camera) {
    for (size_t n = 0; n < chunksLoading.size(); n++) {
        Chunk* chunk;

        chunk = chunksLoading[n];
        chunk->SetPlayerProximity(position); //sqrt take some time
        chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz, 24);
    }

    //instead sort from middle to borders and maybe limit the ammount of chunks in chunksLoading
    std::sort(chunksLoading.begin(), chunksLoading.end(), [](Chunk* a, Chunk* b) {
        return ((a->playerProximity + (!a->isVisible << 10)) < (b->playerProximity + (!b->isVisible << 10)));
     });
}



void WorldArea::loadNewChunks(glm::vec3 &position) {
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
                Chunk::chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
                chunksLoading.push_back(newChunk);
                loadedChunks[x * maxChunk + z] = newChunk;
            }
        }
}

void WorldArea::LoadChunks(glm::vec3 &position, Camera& camera) {

    memset(loadedChunks, 0, (DATA_RENDER_DISTANCE << 1) * (DATA_RENDER_DISTANCE << 1) * sizeof(Chunk*));

    fillLoadedChunks(chunks, position);
    fillLoadedChunks(chunksLoading, position);

    loadNewChunks(position);

    sortChunksLoading(position, camera);

    thread.LoadChunk(chunksLoading);
    thread.CreateMesh(chunks, chunksLoading);
}

void WorldArea::setChunksVisibility(Camera &camera) {
    Chunk* chunk;

    for (unsigned n = 0; n < chunks.size(); n++) {
        chunk = chunks[n];
        if (chunk->status == CHUNK_LOADED)
            chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz);
    }
}

void WorldArea::ReloadShader(bool wireframeMode, std::vector<std::string> shaderOption) {
    Shader newShader;
    
    if (wireframeMode)
        newShader.Load(shaderOption, "shaders/cubeVS.glsl", "shaders/wireFrameFS.glsl", "shaders/wireFrameGS.glsl");
    else
        newShader.Load(shaderOption, "shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
    chunkShader.Delete();
    chunkShader = newShader;
}

std::vector<Chunk*>& WorldArea::GetChunks() {
    return chunks;
}

Shader& WorldArea::GetShader() {
    return chunkShader;
}
