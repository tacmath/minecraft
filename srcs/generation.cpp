#include "generation.h"

ChunkGeneration::ChunkGeneration(){
    std::srand((unsigned int)time(0));
    unsigned int seed = 7;
    for (int n = 0; n != 20; ++n) {
        seed = seed * 10 + (1 + std::rand() / ((RAND_MAX + 1u) / 6));
    }
    perlinNoise.setSeed(seed);
    simplexNoise.SetSeed(seed);
}

ChunkGeneration::~ChunkGeneration(){}


void ChunkGeneration::generate(int ChunkSize, int posx, int posz, unsigned char *cubes) {

    oldGenerate(ChunkSize, posx, posz, cubes);
    return ;

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
    (float)perlinNoise.noise(posX * (1.0f / 200.0f * space), posZ * (1.0f / 200.0f * space)) * 0.8f +
    (float)perlinNoise.noise(posX * (1.0f / 100.0f * space), posZ * (1.0f / 100.0f * space)) * 0.3f +
    (float)perlinNoise.noise(posX * (1.0f / 300.0f * space), posZ * (1.0f / 300.0f * space)) * 0.3f +
    (float)perlinNoise.noise(posX * (1.0f / 50.0f * space), posZ * (1.0f / 50.0f * space)) * 0.1f;
    if (value > 1) value = value - 1;

    float select = (float)perlinNoise.noise(posX * (1.0f / 50.0f * space), posZ * (1.0f / 50.0f * space));

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

// radius doit etre impair (c'est mieux)
void ChunkGeneration::caveExcavator(unsigned char *cubes, int x, int y, int z, int ChunkSize, int radius) {
    for (int i = -radius + 1; i < radius - 1; i++) {
        for (int j = -radius + 1; j < radius - 1; j++) {
            for (int k = -radius - 1; k < radius + 1; k++) {
                if (abs(i) + abs(j) + abs(k) <= radius) {
                    if ((x + i) >= 0 && (x + i) < ChunkSize && (z + j) >= 0 && (z + j) < ChunkSize && (y + k) > 0 && (y + k) < 256) {
                        if (i > 0 || j > 0 || k > 0)
                            cubes[GET_CUBE((x + i), (y + k), (z + j))] = -1;
                        cubes[GET_CUBE((x + i), (y + k), (z + j))] = 0;
                    }
                }
            }
        }
    }
}




// radius doit etre impair (c'est mieux)
void ChunkGeneration::dirtPockateExcavator(unsigned char* cubes, int x, int y, int z, int ChunkSize, int radius) {
    for (int i = -radius; i < radius; i++) {
        for (int j = -radius; j < radius; j++) {
            for (int k = -radius - 1; k < radius; k++) {
                if (abs(i) + abs(j) + abs(k) <= radius) {
                    if ((x + i) >= 0 && (x + i) < ChunkSize && (z + j) >= 0 && (z + j) < ChunkSize && (y + k) > 0 && (y + k) < 256) {
                        if (i > 0 || j > 0 || k > 0)
                            cubes[GET_CUBE((x + i), (y + k), (z + j))] = -2;
                        cubes[GET_CUBE((x + i), (y + k), (z + j))] = 2;
                    }
                }
            }
        }
    }
}

void ChunkGeneration::generateCave(int ChunkSize, unsigned char *cubes, int maxHeight, int x, int z, int posx, int posz) {
    float cave = (float)(perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 47.0f), (posz * ChunkSize + z) * (1.0f / 47.0f))
        + perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 33.0f), (posz * ChunkSize + z) * (1.0f / 33.0f)));

    if (cave > 1.15) {
        int y = (int)(perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 130.0f), (posz * ChunkSize + z) * (1.0f / 130.0f))
            * (maxHeight * 2)
            + perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 45.0f), (posz * ChunkSize + z) * (1.0f / 45.0f)) * 10
            - perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 53.0f), (posz * ChunkSize + z) * (1.0f / 53.0f)) * 6
            - maxHeight / 2);
        if (y < 11) y = 11;
        caveExcavator(cubes, x, y, z, ChunkSize, 2);
    }
}


void ChunkGeneration::generateDirtPochet(int ChunkSize, unsigned char* cubes, int maxHeight, int x, int z, int posx, int posz) {
    float isDirtPocket = (float)(perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 53.0f), (posz * ChunkSize + z) * (1.0f / 53.0f))
        + perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 17.0f), (posz * ChunkSize + z) * (1.0f / 17.0f)));

    if ((isDirtPocket > 1.2 && isDirtPocket < 1.25) || (isDirtPocket > 0.7 && isDirtPocket < 0.75) || (isDirtPocket > 0.9 && isDirtPocket < 0.92)) {
        int y = (int)(perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 130.0f), (posz * ChunkSize + z) * (1.0f / 130.0f)) * (maxHeight * 1.8)
            + perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 45.0f), (posz * ChunkSize + z) * (1.0f / 45.0f)) * 10
            - perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 53.0f), (posz * ChunkSize + z) * (1.0f / 53.0f)) * 6
            - maxHeight / 2);
        if (y < 7) y = 7;
        if (y > maxHeight - 7) y = maxHeight - 7;
        dirtPockateExcavator(cubes, x, y, z, ChunkSize, 3);
    }

}

void ChunkGeneration::oldGenerate(int ChunkSize, int posx, int posz, unsigned char *cubes) {
    for (int x = 0; x < ChunkSize; x++) {
        for (int z = 0; z < ChunkSize; z++) {
            int height = groundHeight(posx * ChunkSize + x, posz * ChunkSize + z);
            for (int y = 0; y < height; y++) {
                if (y == 0) cubes[GET_CUBE(x, y, z)] = 5;
                else if ((char)cubes[GET_CUBE(x, y, z)] == -1) cubes[GET_CUBE(x, y, z)] = 0;
                else if ((char)cubes[GET_CUBE(x, y, z)] == -2) cubes[GET_CUBE(x, y, z)] = 2;
                else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 1;
                else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
                else cubes[GET_CUBE(x, y, z)] = 3;
            }
          //  generateOre(ChunkSize, cubes, height, x, z, posx, posz);
            generateDirtPochet(ChunkSize, cubes, height, x, z, posx, posz);
            generateCave(ChunkSize, cubes, height, x, z, posx, posz);
            generateTree(ChunkSize, cubes, height, x, z, posx, posz);
            generateRiver(ChunkSize, cubes, height, x, z, posx, posz);
        }
    }
}

void ChunkGeneration::generateOre(int ChunkSize, unsigned char *cubes, int y, int x, int z, int posx, int posz) {
    float ores = (float)(perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 15.0f), (posz * ChunkSize + z) * (1.0f / 15.0f)) * 0.5f
        + perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 17.0f), (posz * ChunkSize + z) * (1.0f / 17.0f)) * 0.5f);

    if (ores > 0.5) {
        float isOres = (float)perlinNoise.noise((posx * ChunkSize + x) * (1.0f / 2.0f), (posz * ChunkSize + z) * (1.0f / 2.0f));
            if (isOres < 0.25 || isOres > 85) {
                cubes[GET_CUBE(x, y, z)] = 15;
            }
    }
}



void ChunkGeneration::generateRiver(int ChunkSize, unsigned char *cubes, int ground, int x, int z, int posx, int posz) {
    float river = (float)(simplexNoise.noise((posx * ChunkSize + x) * (1.0f / 78.0f), (posz * ChunkSize + z) * (1.0f / 78.0f)) * 0.7f
    + simplexNoise.noise((posx * ChunkSize + x) * (1.0f / 22.0f), (posz * ChunkSize + z) * (1.0f / 22.0f)) * 0.3f);
    if (river > 0.8 && ground < 50 && ground > 45) {
                for (int i = 40; i < 45; i++) {
                    cubes[GET_CUBE(x, i, z)] = 23;
                }
                for (int i = 45; i < 50; i++) {
                    cubes[GET_CUBE(x, i, z)] = 0;
                }
    }
}

void  ChunkGeneration::treeModel(int ChunkSize, int x, int y, int z, unsigned char *cubes) {
    
    
    for (int xx = -3; xx <= 3 ; xx++) {
        for (int yy = -1; yy <= 4 ; yy++) {
            for (int zz = -3; zz <= 3 ; zz++) {
                if (glm::length(glm::vec3((float)xx,(float) yy, (float)zz)) <= 3.0) {
                    if (x + xx < ChunkSize && x + xx >= 0 && z + zz < ChunkSize && z + zz >= 0) {
                        cubes[GET_CUBE((x + xx), (y + yy + 4), (z + zz))] = 6;
                    }
                }
            }
        }
    }
    if (x < ChunkSize && x  >= 0 && z  < ChunkSize && z >= 0) {
        cubes[GET_CUBE(x, y, z)] = 7;
        cubes[GET_CUBE(x, (y + 1), z)] = 7;
        cubes[GET_CUBE(x, (y + 2), z)] = 7;
        cubes[GET_CUBE(x, (y + 3), z)] = 7;
        cubes[GET_CUBE(x, (y + 4), z)] = 7;
    }
}

void ChunkGeneration::generateTree(int ChunkSize, unsigned char *cubes, int ground, int x, int z, int posx, int posz) {

    for (int overflowX = -2; overflowX <= 2; overflowX++) {
        for (int overflowZ = -2; overflowZ <= 2; overflowZ++) {
            float tree = (float)(perlinNoise.noise((posx * ChunkSize + x + overflowX) * (1.0f / 78.0f), (posz * ChunkSize + z + overflowZ) * (1.0f / 78.0f)) * 0.7f
            + perlinNoise.noise((posx * ChunkSize + x + overflowX) * (1.0f / 22.0f), (posz * ChunkSize + z + overflowZ) * (1.0f / 22.0f)) * 0.3f);
            if (tree > 0.5) {
                float isTree = (float)perlinNoise.noise((posx * ChunkSize + x + overflowX) * (1.0f / 2.0f), (posz * ChunkSize + z + overflowZ) * (1.0f / 2.0f));
                if (isTree < 0.25 || isTree > 85) {
                    treeModel(ChunkSize, x + overflowX, ground, z + overflowZ, cubes);
                }
            }
        }
    }
}


int ChunkGeneration::groundHeight(int x, int z) {
    float selector = (float)perlinNoise.noise(x * (1.0f / 40.0f), z * (1.0f / 40.0f));
    int height = 0;

    if (selector  > 0.5) {
        height += (int)(4 * selector);
    }

    return 30 + height + (int)(perlinNoise.noise(x * (1.0f / 500.0f), z * (1.0f / 500.0f)) * 40
    + perlinNoise.noise(x * (1.0f / 50.0f), z * (1.0f / 50.0f)) * 15
    + perlinNoise.noise(x * (1.0f / 20.0f), z * (1.0f / 20.0f)) * 7
    - perlinNoise.noise(x * (1.0f / 300.0f), z * (1.0f / 300.0f)) * 30);
}