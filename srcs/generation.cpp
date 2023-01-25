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


void ChunkGeneration::generate(int ChunkSize, int posx, int posz, unsigned char *cubes) {
    for (int x = 0; x < ChunkSize; x++) {
        for (int z = 0; z < ChunkSize; z++) {
            generateBiome((posx << 4) + x, (posz << 4) + z, cubes);
        }
    }
}


/*
    for (int y = 0; y < height; y++) {
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if ((char)cubes[GET_CUBE(x, y, z)] == -1) cubes[GET_CUBE(x, y, z)] = 0;
        else if ((char)cubes[GET_CUBE(x, y, z)] == -2) cubes[GET_CUBE(x, y, z)] = 2;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 1;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
*/


void ChunkGeneration::generateBiome(long posX, long posZ, unsigned char *cubes) {
    float space = 0.5;
   
    float value =
    perlinNoise.noise(posX * (1.0f / 200.0f * space), posZ * (1.0f / 200.0f * space)) * 0.8 +
    perlinNoise.noise(posX * (1.0f / 100.0f * space), posZ * (1.0f / 100.0f * space)) * 0.3 +
    perlinNoise.noise(posX * (1.0f / 300.0f * space), posZ * (1.0f / 300.0f * space)) * 0.3 + 
    perlinNoise.noise(posX * (1.0f / 50.0f * space), posZ * (1.0f / 50.0f * space)) * 0.1;
    if (value > 1) value = value - 1;

    float select = perlinNoise.noise(posX * (1.0f / 50.0f * space), posZ * (1.0f / 50.0f * space));

    char x = posX & 0xF;
    char z = posZ & 0xF;


    int height = groundHeight(posX, posZ);


    if (select < 0.5) {
        if (value < 0.2) cubes[GET_CUBE(x, height, z)] = 1;         ///  generateMarais
        else if (value < 0.28) cubes[GET_CUBE(x, height, z)] = 9;   ///  RANDOM
        else if (value < 0.36) cubes[GET_CUBE(x, height, z)] = 7;   /// generateMer
        else if (value < 0.44) cubes[GET_CUBE(x, height, z)] = 23;   ///  generateDesert
        else if (value < 0.52) cubes[GET_CUBE(x, height, z)] = 3;  ///  generateNeige
        else if (value < 0.6) cubes[GET_CUBE(x, height, z)] =  11;   ///  generateForet
        else if (value < 0.68) cubes[GET_CUBE(x, height, z)] = 8;   ///  plaine
        else if (value < 0.76) cubes[GET_CUBE(x, height, z)] = 4;  ///  generateMer
        else if (value < 0.84) cubes[GET_CUBE(x, height, z)] = 23;  ///  generateMer
        else cubes[GET_CUBE(x, height, z)] = 5;                     ///  generateMontagne
    }

    if (value < 0.2) cubes[GET_CUBE(x, height, z)] = 5;         ///  generateMarais
    else if (value < 0.28) cubes[GET_CUBE(x, height, z)] = 24;   ///  RANDOM
    else if (value < 0.36) cubes[GET_CUBE(x, height, z)] = 23;   /// generateMer
    else if (value < 0.44) cubes[GET_CUBE(x, height, z)] = 9;   ///  generateDesert
    else if (value < 0.52) cubes[GET_CUBE(x, height, z)] = 11;  ///  generateNeige
    else if (value < 0.6) cubes[GET_CUBE(x, height, z)] =  7;   ///  generateForet
    else if (value < 0.68) cubes[GET_CUBE(x, height, z)] = 1;   ///  plaine
    else if (value < 0.76) cubes[GET_CUBE(x, height, z)] = 23;  ///  generateMer
    else if (value < 0.84) cubes[GET_CUBE(x, height, z)] = 24;  ///  generateMer
    else cubes[GET_CUBE(x, height, z)] = 3;                     ///  generateMontagne
}

void    ChunkGeneration::generatePlaine(long posX, long posZ, unsigned char *cubes){
    int height = groundHeight(posX, posZ);
    for (int y = 0; y < height; y++) {
        char x = posX & 0xF;
        char z = posZ & 0xF;
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 1;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
}
void    ChunkGeneration::generateDesert(long posX, long posZ, unsigned char *cubes){
    int height = groundHeight(posX, posZ);
    for (int y = 0; y < height; y++) {
        char x = posX & 0xF;
        char z = posZ & 0xF;
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 9;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 10;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
}

void    ChunkGeneration::generateMontagne(long posX, long posZ, unsigned char *cubes){
        int height = groundHeight(posX, posZ);
        for (int y = 0; y < height; y++) {
        char x = posX & 0xF;
        char z = posZ & 0xF;
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 3;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
}
void    ChunkGeneration::generateMer(long posX, long posZ, unsigned char *cubes){}


void    ChunkGeneration::generateMarais(long posX, long posZ, unsigned char *cubes){
    int height = groundHeight(posX, posZ);
    for (int y = 0; y < height; y++) {
        char x = posX & 0xF;
        char z = posZ & 0xF;
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 23;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
}


void    ChunkGeneration::generateForet(long posX, long posZ, unsigned char *cubes){
    int height = groundHeight(posX, posZ);
    for (int y = 0; y < height; y++) {
        char x = posX & 0xF;
        char z = posZ & 0xF;
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 6;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
}


void    ChunkGeneration::generateNeige(long posX, long posZ, unsigned char *cubes){
    int height = groundHeight(posX, posZ);
    for (int y = 0; y < height; y++) {
        char x = posX & 0xF;
        char z = posZ & 0xF;
        if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
        else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 11;
        else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
        else cubes[GET_CUBE(x, y, z)] = 3;
    }
}

int ChunkGeneration::groundHeight(long x, long z) {
    int height = 40
    + perlinNoise.noise(x * (1.0f / 200.0f), z * (1.0f / 200.0f)) * 20
    + perlinNoise.noise(x * (1.0f / 15.0f), z * (1.0f / 15.0f)) * 5
    + perlinNoise.noise(x * (1.0f / 100.0f), z * (1.0f / 100.0f)) * 14;
    return height;
}