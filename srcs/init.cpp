#include "world_area.h"

void WorldArea::initChunks(int radius) {
    long diameter = (radius + 1) * 2;

    chunksLoading.resize(diameter * diameter);
    for (int x = 0; x < diameter; x++)
        for (int z = 0; z < diameter; z++) {
            Chunk *newChunk = new Chunk;
            newChunk->SetPosistion(x - (radius + 1), z - (radius + 1));
            Chunk::chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
            newChunk->Generate();
            chunksLoading[x * diameter + z] = newChunk;
        }
    for (int x = 1; x < diameter - 2; x++)
        for (int z = 1; z < diameter - 2; z++) {
            Chunk* chunk = chunksLoading[x * diameter + z];
            chunk->createMeshData();
            chunk->Bind();
            chunk->isVisible = true;
            chunks.push_back(chunk);
            chunksLoading.erase(chunksLoading.begin() + x * diameter + z);
        }
}

void WorldArea::initUniforms(Camera &camera) {
    chunkShader.Activate();
    chunkShader.setInt("atlas", 0);
    chunkShader.setMat4("projection", camera.projection);
    chunkShader.setMat4("view", camera.view);
}