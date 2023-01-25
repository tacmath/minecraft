#include "generation.h"

ChunkGeneration::ChunkGeneration(){
    std::srand((unsigned int)std::time(0));
    unsigned int seed = 7;
    for (int n = 0; n != 20; ++n) {
        seed = seed * 10 + (1 + std::rand() / ((RAND_MAX + 1u) / 6));
    }
    perlinNoise.setSeed(seed);
    simplexNoise.SetSeed(seed);
}

ChunkGeneration::~ChunkGeneration(){}


Biome ChunkGeneration::biome(int ChunkSize, int posx, int posz) {
    return Biome::plaine;
}

void ChunkGeneration::generate(int ChunkSize, int posx, int posz, unsigned char *cubes) {
    for (int x = 0; x < ChunkSize; x++) {
        for (int z = 0; z < ChunkSize; z++) {
            int height = groundHeight((posx << 4) + x, (posz << 4) + z);
            for (int y = 0; y < height; y++) {
                if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
                else if ((char)cubes[GET_CUBE(x, y, z)] == -1) cubes[GET_CUBE(x, y, z)] = 0;
                else if ((char)cubes[GET_CUBE(x, y, z)] == -2) cubes[GET_CUBE(x, y, z)] = 2;
                else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 1;
                else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
                else cubes[GET_CUBE(x, y, z)] = 3;
            }
        }
    }
}

int ChunkGeneration::groundHeight(int x, int z) {
    int height = 30 + perlinNoise.noise(x * (1.0f / 100.0f), z * (1.0f / 100.0f)) * 4;
    return height;
}