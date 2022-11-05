#include "minecraft.h"

Minecraft::Minecraft(void) {
    window = 0;
    initWindows();
    initSkybox();
    skyboxShader.Load("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
    chunkShader.Load("shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
    camera.Init((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, glm::vec3(0.0f, 60.0f, 0.0f));

    std::vector<std::string> textureNames = { "texture/grass_side.png", "texture/grass_top.png", "texture/dirt.png", "texture/stone.png" };

    texAtlas.LoadAtlas(textureNames, 0);
    enableGlParam();
    initUniforms();

    blocks[1].SetTextures(1, 0, 2);
    blocks[2].SetTextures(2, 2, 2);
    blocks[3].SetTextures(3, 3, 3);

    seed = (int)((double)rand() / (RAND_MAX)) * UINT32_MAX;
    global_noise.SetSeed(seed);
    globalChunkGeneration.SetSeed(seed);

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
        //     if (camera.frustum.isVisible((float)(chunks[n]->posx << 4), (float)(chunks[n]->posz << 4), CHUNK_SIZE))
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

void Minecraft::LoadChunks() {
    Chunk    *loadedChunks[RENDER_DISTANCE << 1][RENDER_DISTANCE << 1];     //probably need to change that for a dynamic table
    int x, z, playerPosx, playerPosz, maxChunk;
    int n, chunkNumber;
    maxChunk = RENDER_DISTANCE << 1;
    playerPosx = ((int)camera.posision.x >> 4) - RENDER_DISTANCE;
    playerPosz = ((int)camera.posision.z >> 4) - RENDER_DISTANCE;
    memset(loadedChunks, 0, maxChunk * maxChunk * sizeof(Chunk*));
    chunkNumber = (int)chunks.size();
    n = -1;
    while (++n < chunkNumber) {
        x = chunks[n]->posx - playerPosx;
        z = chunks[n]->posz - playerPosz;

        // check if a chunk is too far away and delete it if nessesary
        if ((x < -UNLOAD_OFFSET || z < -UNLOAD_OFFSET || x > maxChunk + UNLOAD_OFFSET || z > maxChunk + UNLOAD_OFFSET) && chunks[n]->threadStatus == CHUNK_NOT_PROCESSING) {
            delete chunks[n];
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
            if (!loadedChunks[x][z]) {
                Chunk* newChunk = new Chunk;     //push_back is creating a copy
                newChunk->SetPosistion(playerPosx + x, playerPosz + z);
                chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
                thread.AddChunk(newChunk);
                /*newChunk->Generate();
                newChunk->createMeshData();
                newChunk->Bind();*/
                chunks.push_back(newChunk);     //if needed push_back fist the most important chunk or create a priority list
                loadedChunks[x][z] = newChunk;
                continue;
            }

            // load the chunk border if it is not being processed and has cube data
            if (loadedChunks[x][z]->neighbourLoaded != CHUNK_ALL_LOADED && loadedChunks[x][z]->status >= CHUNK_DATA_LOADED && !(loadedChunks[x][z]->threadStatus & CHUNK_PROCESSING))
                loadedChunks[x][z]->addNeighbours();

            // add chunks to thread that where not able to be processed
            if (loadedChunks[x][z]->status == CHUNK_UNLOADED && !(loadedChunks[x][z]->threadStatus & CHUNK_PROCESSING))
                thread.AddChunk(loadedChunks[x][z]);
        }
}