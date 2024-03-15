#include "world_area.h"
#include <algorithm>

void WorldArea::initChunks(unsigned radius) { // maybe remake the function to use some multithreading
    unsigned long diameter = (radius + 1) * 2;

    chunksLoading.resize(diameter * diameter);
    for (unsigned x = 0; x < diameter; x++)
        for (unsigned z = 0; z < diameter; z++) {
            Chunk *newChunk = new Chunk; //maybe use shared_ptr for more safety just test the perfs
            newChunk->SetPosistion(x - (radius + 1), z - (radius + 1));
            Chunk::chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
            newChunk->Generate();
            chunksLoading[x * diameter + z] = newChunk;
        }
    chunks.reserve((diameter - 2) * (diameter - 2));
    for (unsigned x = 1; x < diameter - 2; x++)
        for (unsigned z = 1; z < diameter - 2; z++) {
            Chunk* chunk = chunksLoading[x * diameter + z];
            chunk->createMeshData();
            chunk->Bind();
            chunk->isVisible = true;
            chunks.push_back(chunk);
            chunksLoading[x * diameter + z] = 0;
        }

    // remove all chunks that have been loaded
    auto iterator = std::remove_if(chunksLoading.begin(), chunksLoading.end(), [](Chunk* chunk) { return chunk == 0; });
    chunksLoading.erase(iterator, chunksLoading.end());
}

// initialize the texure atlas, uniform and chunks

void WorldArea::Init(const Camera& camera, Sound& sound) {
    std::vector<std::string> textureNames;

    parseConfigs(textureNames, sound);
    Chunk::blocks[AIR].visibility == VISIBILITY::TRANSPARENT;
    for (auto& name : textureNames)
        name = "texture/" + name;
    texAtlas.LoadArray(textureNames, 0);

    initUniforms(camera);
    initChunks(STARTING_RENDER_DISTANCE);
}

void WorldArea::initUniforms(const Camera &camera) {
    chunkShader.setInt("atlas", 0);
    chunkShader.setInt("shadowMap", 3);
    chunkShader.setMat4("projection", camera.projection);
    chunkShader.setMat4("view", camera.view);
}