#include "world_area.h"

void WorldArea::initChunks(unsigned radius) {
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
    for (unsigned x = 1; x < diameter - 2; x++)
        for (unsigned z = 1; z < diameter - 2; z++) {
            Chunk* chunk = chunksLoading[x * diameter + z];
            chunk->createMeshData();
            chunk->Bind();
            chunk->isVisible = true;
            chunks.push_back(chunk);
            chunksLoading.erase(chunksLoading.begin() + x * diameter + z);
        }
    unsigned test = 0;
}

// initialize the texure atlas, uniform and chunks

void WorldArea::Init(const Camera& camera/*, std::vector<SoundBuffers>& sounds*/) {
    std::vector<std::string> textureNames;

    parseBlockData(textureNames);
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