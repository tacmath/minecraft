#ifndef CHUNK_GENERATION_CLASS_H
#define CHUNK_GENERATION_CLASS_H

#include "noise.h"

// get the offset of a cube based on the position in the chunk
#define GET_CUBE(x, y, z) ((y << 8) | (x << 4) | z)

class ChunkGeneration {

    public:
    ChunkGeneration(){}
    ~ChunkGeneration(){}

    // radius doit etre impair (c'est mieux)
    void caveExcavator(unsigned char *cubes, int x, int y, int z, int ChunkSize, int radius) {
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
    void dirtPockateExcavator(unsigned char* cubes, int x, int y, int z, int ChunkSize, int radius) {
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

    void generateCave(int ChunkSize, unsigned char *cubes, int maxHeight, int x, int z, int posx, int posz) {
        float cave = global_noise.noise((posx * ChunkSize + x) * (1.0f / 47.0f), (posz * ChunkSize + z) * (1.0f / 47.0f))
            + global_noise.noise((posx * ChunkSize + x) * (1.0f / 33.0f), (posz * ChunkSize + z) * (1.0f / 33.0f));

        if (cave > 1.15) {
            int y = global_noise.noise((posx * ChunkSize + x) * (1.0f / 130.0f), (posz * ChunkSize + z) * (1.0f / 130.0f))
                * (maxHeight * 2)
                + global_noise.noise((posx * ChunkSize + x) * (1.0f / 45.0f), (posz * ChunkSize + z) * (1.0f / 45.0f)) * 10
                - global_noise.noise((posx * ChunkSize + x) * (1.0f / 53.0f), (posz * ChunkSize + z) * (1.0f / 53.0f)) * 6
                - maxHeight / 2;
            if (y < 11) y = 11;
            caveExcavator(cubes, x, y, z, ChunkSize, 2);
        }

    }


    void generateDirtPochet(int ChunkSize, unsigned char* cubes, int maxHeight, int x, int z, int posx, int posz) {
        float isDirtPocket = global_noise.noise((posx * ChunkSize + x) * (1.0f / 53.0f), (posz * ChunkSize + z) * (1.0f / 53.0f))
            + global_noise.noise((posx * ChunkSize + x) * (1.0f / 17.0f), (posz * ChunkSize + z) * (1.0f / 17.0f));

        if ((isDirtPocket > 1.2 && isDirtPocket < 1.25) || (isDirtPocket > 0.7 && isDirtPocket < 0.75) || (isDirtPocket > 0.9 && isDirtPocket < 0.92)) {
            int y = global_noise.noise((posx * ChunkSize + x) * (1.0f / 130.0f), (posz * ChunkSize + z) * (1.0f / 130.0f)) * (maxHeight * 1.8)
                + global_noise.noise((posx * ChunkSize + x) * (1.0f / 45.0f), (posz * ChunkSize + z) * (1.0f / 45.0f)) * 10
                - global_noise.noise((posx * ChunkSize + x) * (1.0f / 53.0f), (posz * ChunkSize + z) * (1.0f / 53.0f)) * 6
                - maxHeight / 2;
            if (y < 7) y = 7;
            if (y > maxHeight - 7) y = maxHeight - 7;
            dirtPockateExcavator(cubes, x, y, z, ChunkSize, 3);
        }

    }

    void generate(int ChunkSize, int posx, int posz, unsigned char *cubes) {
		for (unsigned x = 0; x < ChunkSize; x++) {
			for (unsigned z = 0; z < ChunkSize; z++) {
				unsigned height = groundHeight(global_noise, posx * ChunkSize + x, posz * ChunkSize + z);
				for (unsigned y = 0; y < height; y++) {
                    if (cubes[GET_CUBE(x, y, z)] == -1) cubes[GET_CUBE(x, y, z)] = 0;
                    else if (cubes[GET_CUBE(x, y, z)] == -2) cubes[GET_CUBE(x, y, z)] = 2;
                    else if (y == height - 1) cubes[GET_CUBE(x, y, z)] = 1;
                    else if (y > height - 4) cubes[GET_CUBE(x, y, z)] = 2;
                    else cubes[GET_CUBE(x, y, z)] = 3;
				}
                generateDirtPochet(ChunkSize, cubes, height, x, z, posx, posz);
                generateCave(ChunkSize, cubes, height, x, z, posx, posz);
			}
		}
    }

    private:

	inline int groundHeight(Noise& noise, int x, int z) {
            int height = 30 + (int)(noise.noise(x * (1.0f / 500.0f), z * (1.0f / 500.0f)) * 25
			+ noise.noise(x * (1.0f / 50.0f), z * (1.0f / 50.0f)) * 15
			+ noise.noise(x * (1.0f / 20.0f), z * (1.0f / 20.0f)) * 7
			- noise.noise(x * (1.0f / 300.0f), z * (1.0f / 300.0f)) * 30);

            float nice =  noise.noise(x * (1.0f / 30.0f), z * (1.0f / 30.0f));
            if (nice < 0.5) nice = 0.5;
            height += (int)(nice * 30);
           return height;
	}


	inline int getRadius(Noise& noise, int x, int z, int y) {
			return 3 + (int)(noise.noise(x * (1.0f / 40.0f), z * (1.0f / 40.0f)) * 1
                    + noise.noise(x * (1.0f / 100.0f), z * (1.0f / 100.0f)) * 1
                    + noise.noise(x * (1.0f / 20.0f), z * (1.0f / 20.0f)) * 1
                    - noise.noise(x * (1.0f / 62.0f), z * (1.0f / 62.0f)) * 1);
	}

    inline int getHeight(Noise& noise, int x, int z, int y) {
			return 40 
            + (int)(noise.noise(x * (1.0f / 140.0f), z * (1.0f / 140.0f)) * 40
            - noise.noise(x * (1.0f / 75.0f), z * (1.0f / 75.0f)) * 20
            - noise.noise(x * (1.0f / 10.0f), z * (1.0f / 10.0f)) * 5
            + noise.noise(x * (1.0f / 75.0f), z * (1.0f / 75.0f)) * 10);
    }
};

extern ChunkGeneration globalChunkGeneration;

#endif