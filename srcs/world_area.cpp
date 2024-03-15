#include "world_area.h"

WorldArea::WorldArea(void) {
    std::vector<std::string> shaderOption;
    shaderOption.push_back("SHADOW");

    chunkShader.Load(shaderOption, "shaders/cubeVS.glsl", "shaders/cubeFS.glsl");

    dataLoadDistance = DATA_LOAD_DISTANCE(RENDER_DISTANCE);
    loadedChunks = (Chunk**)calloc((dataLoadDistance << 1) * (dataLoadDistance << 1), sizeof(Chunk*));
    if (!loadedChunks)
        exit(1);
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

void WorldArea::Draw(void) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    chunkShader.Activate();
    for (size_t n = 0; n < chunks.size(); n++)
        chunks[n]->DrawVisible(chunkShader);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}


//load the view matrix in all the shaders
void WorldArea::LoadViewMatrix(const Camera& camera) {
    chunkShader.setMat4("view", camera.view);
}


void WorldArea::fillLoadedChunks(std::vector<Chunk*>& chunks, const glm::vec3& position) {
    int playerPosx, playerPosz, maxChunk;

    maxChunk = dataLoadDistance << 1;
    playerPosx = ((int)position.x >> 4) - dataLoadDistance;
    playerPosz = ((int)position.z >> 4) - dataLoadDistance;
    auto it = std::remove_if(chunks.begin(), chunks.end(), [&](Chunk* chunk) {
        int x, z;

        x = chunk->posx - playerPosx;
        z = chunk->posz - playerPosz;
        // check if a chunk is too far away and delete it if nessesary
        if((x < -UNLOAD_OFFSET || z < -UNLOAD_OFFSET || x > maxChunk + UNLOAD_OFFSET || z > maxChunk + UNLOAD_OFFSET) && chunk->threadStatus == CHUNK_NOT_PROCESSING) {
            delete chunk;
            return true;
        }
        // fill the 2d array if a chunk is in the RENDER_DISTANCE
        if (x >= 0 && z >= 0 && x < maxChunk && z < maxChunk)
            loadedChunks[x * maxChunk + z] = chunk;
        return false;
    });
    if (it != chunks.end())
        chunks.erase(it, chunks.end());
}


void WorldArea::sortChunksLoading(const glm::vec3& position, const Camera& camera) {
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

void WorldArea::sortChunks(const glm::vec3& position) {
    for (size_t n = 0; n < chunks.size(); n++)
        chunks[n]->playerProximity = abs(chunks[n]->posx - ((int)position.x >> 4)) + abs(chunks[n]->posz - ((int)position.z >> 4));

    std::sort(chunks.begin(), chunks.end(), [](Chunk* a, Chunk* b) {
        return (a->playerProximity > b->playerProximity);
        });
}



void WorldArea::loadNewChunks(const glm::vec3 &position) {
    int playerPosx, playerPosz, maxChunk;

    maxChunk = dataLoadDistance << 1;
    playerPosx = ((int)position.x >> 4) - dataLoadDistance;
    playerPosz = ((int)position.z >> 4) - dataLoadDistance;
    //add all the chunk that are in loadedChunks but didn't exist
    for (int x = 0; x < maxChunk; x++)
        for (int z = 0; z < maxChunk; z++) {
            // load new chunks if it is in the render distance but didn't exist
            if (!loadedChunks[x * maxChunk + z] /*&& VEC2_LEN((x - dataLoadDistance), (z - dataLoadDistance)) <= dataLoadDistance*/) {
                Chunk* newChunk = new Chunk;
                newChunk->SetPosistion(playerPosx + x, playerPosz + z);
                Chunk::chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk; //maybe add it to SetPosistion
                chunksLoading.push_back(newChunk);
                loadedChunks[x * maxChunk + z] = newChunk;
            }
        }
}

void WorldArea::LoadChunks(const glm::vec3 &position, const Camera& camera) {

    memset(loadedChunks, 0, (dataLoadDistance << 1) * (dataLoadDistance << 1) * sizeof(Chunk*));

    fillLoadedChunks(chunks, position);
    fillLoadedChunks(chunksLoading, position);

    loadNewChunks(position);

    sortChunksLoading(position, camera);
    sortChunks(position);

    thread.LoadChunk(chunksLoading);
    thread.CreateMesh(chunks, chunksLoading);
}

/*
* to make it faster split the area in 4 parts 
* 
* if all the area in frustum then all chunks of the area are visible
* if the area is on no plane of the frustum then all chunks of the area are not visible
* if not the above then split the area in 4 and try again until an area is a chunk wide
*/
void WorldArea::setChunksVisibility(const Camera &camera) {
    Chunk* chunk;

    for (unsigned n = 0; n < chunks.size(); n++) {
        chunk = chunks[n];
        if (chunk->status == CHUNK_LOADED)
            chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz);
    }
}

void WorldArea::UpdateRenderDistance(unsigned newRenderDistance) {
    dataLoadDistance = DATA_LOAD_DISTANCE(newRenderDistance);
    free(loadedChunks);
    loadedChunks = (Chunk**)calloc((dataLoadDistance << 1) * (dataLoadDistance << 1), sizeof(Chunk*));
    if (!loadedChunks)
        exit(1);
}

void WorldArea::ReloadShader(bool wireframeMode, std::vector<std::string> shaderOption) {
    Shader newShader;
    
    if (wireframeMode)
        newShader.Load(shaderOption, "shaders/cubeVS.glsl", "shaders/wireFrameFS.glsl", "shaders/wireFrameGS.glsl");
    else
        newShader.Load(shaderOption, "shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
    chunkShader = std::move(newShader);
}

std::vector<Chunk*>& WorldArea::GetChunks() {
    return chunks;
}

Shader& WorldArea::GetShader() {
    return chunkShader;
}
