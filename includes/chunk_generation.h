#ifndef CHUNK_GENERATION_CLASS_H
#define CHUNK_GENERATION_CLASS_H

#include "noise.h"

// get the offset of a cube based on the position in the chunk
#define GET_CUBE(x, y, z) ((y << 8) | (x << 4) | z)

class ChunkGeneration {

    public:
    ChunkGeneration(){}
    ~ChunkGeneration(){}


    void propagateCavePoint(unsigned char *cubes, int x, int y, int z) {

    }

    void generateCave(int ChunkSize, unsigned char *cubes, int maxHeight, int x, int z, int posx, int posz) {
        float cave = global_noise.noise((posx * ChunkSize + x) * (1.0f / 50.0f), (posz * ChunkSize + z) * (1.0f / 50.0f))
            + global_noise.noise((posx * ChunkSize + x) * (1.0f / 25.0f), (posz * ChunkSize + z) * (1.0f / 25.0f));

        int height = glm::abs(posx + posz + seed) % maxHeight + 3;
        int radius = glm::abs(posx + posz + seed) % 7;
        if (cave > 1.1 && cave < 1.3) {
            cubes[GET_CUBE(x, height, z)] = 0;
            propagateCavePoint(cubes, x, height, z);
        }
    }


    void generate(int ChunkSize, int posx, int posz, unsigned char *cubes) {
		for (unsigned x = 0; x < ChunkSize; x++) {
			for (unsigned z = 0; z < ChunkSize; z++) {
				unsigned height = groundHeight(global_noise, posx * ChunkSize + x, posz * ChunkSize + z);
				for (unsigned y = 0; y < 256; y++) {
                    if (y < height) cubes[GET_CUBE(x, y, z)] = 1;
				}
                generateCave(ChunkSize, cubes, height, x,  z, posx, posz);
			}
		}
    }

    void SetSeed(unsigned seed) {
        this->seed = seed;
    }

    private:


    unsigned int seed;


	inline int groundHeight(Noise& noise, int x, int z) {
            int height = 30 + noise.noise(x * (1.0f / 500.0f), z * (1.0f / 500.0f)) * 25
			+ noise.noise(x * (1.0f / 50.0f), z * (1.0f / 50.0f)) * 15
			+ noise.noise(x * (1.0f / 20.0f), z * (1.0f / 20.0f)) * 7
			- noise.noise(x * (1.0f / 300.0f), z * (1.0f / 300.0f)) * 30;

            float nice =  noise.noise(x * (1.0f / 30.0f), z * (1.0f / 30.0f));
            if (nice < 0.5) nice = 0.5;
            height += nice * 30;
           return height;
	}


	inline int getRadius(Noise& noise, int x, int z, int y) {
			return 3 + noise.noise(x * (1.0f / 40.0f), z * (1.0f / 40.0f)) * 1
                    + noise.noise(x * (1.0f / 100.0f), z * (1.0f / 100.0f)) * 1
                    + noise.noise(x * (1.0f / 20.0f), z * (1.0f / 20.0f)) * 1
                    - noise.noise(x * (1.0f / 62.0f), z * (1.0f / 62.0f)) * 1;
	}

    inline int getHeight(Noise& noise, int x, int z, int y) {
			return 40 
            + noise.noise(x * (1.0f / 140.0f), z * (1.0f / 140.0f)) * 40
            - noise.noise(x * (1.0f / 75.0f), z * (1.0f / 75.0f)) * 20
            - noise.noise(x * (1.0f / 10.0f), z * (1.0f / 10.0f)) * 5
            + noise.noise(x * (1.0f / 75.0f), z * (1.0f / 75.0f)) * 10;
    }
};

ChunkGeneration globalChunkGeneration;

#endif