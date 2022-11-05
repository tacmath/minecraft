#include "minecraft.h"

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

    //add all the chunk that are not in loadedChunks
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