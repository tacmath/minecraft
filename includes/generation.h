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
    int     groundHeight(long x, long z);

    void    generateBiome(long posX, long posZ, unsigned char *cubes);
    void    generatePlaine(long posX, long posZ, unsigned char *cubes);
    void    generateDesert(long posX, long posZ, unsigned char *cubes);
    void    generateMontagne(long posX, long posZ, unsigned char *cubes);
    void    generateMer(long posX, long posZ, unsigned char *cubes);
    void    generateMarais(long posX, long posZ, unsigned char *cubes);
    void    generateForet(long posX, long posZ, unsigned char *cubes);
    void    generateNeige(long posX, long posZ, unsigned char *cubes);
};

#endif