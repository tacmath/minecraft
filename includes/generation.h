#ifndef CHUNK_GENERATION_CLASS_H
#define CHUNK_GENERATION_CLASS_H

#include "perlinNoise.h"
#include "simplex_noise.h"

// get the offset of a cube based on the position in the chunk
#define GET_CUBE(x, y, z) ((y << 8) | (x << 4) | z)

enum Biome {plaine, desert, montagne, mer, marais, foret, neige };

class ChunkGeneration {

    PerlinNoise         perlinNoise;
    SimplexNoise        simplexNoise;

    public:
    ChunkGeneration();
    ~ChunkGeneration();

    void generate(int ChunkSize, int posx, int posz, unsigned char *cubes);

    private:
    Biome biome(int ChunkSize, int posx, int posz);
    int groundHeight(int x, int z);
};

extern ChunkGeneration globalChunkGeneration;

#endif