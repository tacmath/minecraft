#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include<glad/glad.h>
#include <VAO.h>
#include <noise.h>
#include<vector>


// id of air
#define AIR 0
#define CHUNK_SIZE 16

/*
	store multiple data in an int32 with the format :

	0000	|	0000	|	0000	|	0	|	0	|	00000	|	00000	|	00000000
	unused	| texatlasY	| texatlasX	| textY	| textX	|	posZ	|	posX	|	  posY
*/
#define PACK_VERTEX_DATA(x, y, z, textx, texty) (y | (x << 8) | (z << 13) | textx << 18 | texty << 19)
#define PACK_ATLAS_VERTEX_DATA(texatlasX, texatlasY) ((texatlasX << 20) | (texatlasY << 24))

typedef unsigned char t_cubes[256][CHUNK_SIZE][CHUNK_SIZE];

#include <bitset>


void printVertexData(unsigned int vertex) {
	std::bitset<32> bit(vertex);
	std::cout << "y = " << (vertex & 0xFFu) << " x = " << (((vertex >> 8u) & 0xFu)) << " z = " << (((vertex >> 12u) & 0xFu)) << std::endl;
	std::cout << bit << '\n';
}

class Chunk
{
public:
	int posx;
	int posz;

	// ids of all the cube in the chunk
	t_cubes *cubes;

	//the mesh is an array containing vertex position and other information in an unsigned int
	std::vector<unsigned int> mesh;
	// the number of all the vertices
	unsigned int verticesNumber;

	// vertex array object
	VAO VAO;
	// vertex buffer object ID
	GLuint VBO;

	// Default constructor
	Chunk() {
	//	std::cout << "constructor called" << std::endl;
		verticesNumber = 0;
		posx = 0;
		posz = 0;
		cubes = 0;
		VBO = 0;
		mesh.resize(0);
	};

	// Destructor
	~Chunk() {
	//	std::cout << "chunk has been destroyed" << std::endl;
	//	std::cout << "destructor called  and addr = " << cubes << "  x = " << posx << "  z = " << posz << std::endl;
		free(cubes);
		glDeleteBuffers(1, &VBO);
	}

	void SetPosistion(int x, int z) {
	//	std::cout << "SetPosistion called  and addr = " << cubes << "x = " << posx << "z = " << posz << std::endl;
		posx = x;
		posz = z;
	}

	void Generate(Noise &noise) {
		cubes = (t_cubes*)calloc(1, sizeof(t_cubes));

		for (unsigned x = 0; x < CHUNK_SIZE; x++) {
			for (unsigned z = 0; z < CHUNK_SIZE; z++) {
				unsigned height = getHeight(noise, posx * CHUNK_SIZE + x, posz * CHUNK_SIZE + z);
				for (unsigned y = 0; y < 256; y++)
					if (y < height)
						(*cubes)[y][x][z] = 1;
			}
		}

		createMeshData(*cubes);
		verticesNumber = (unsigned int)mesh.size();
		if (verticesNumber)
			Bind();
		/*else {
			std::cout << "weird chunk generated investigate" << std::endl;
			std::cout << "posx = " << posx << " posz = " << posz << " cubes addr = " << cubes << " mesh size = " << mesh.size() << std::endl;
		}*/
		/*for (unsigned n = 0; n < (verticesNumber / 3); n++) {
			std::cout << std::endl << "triangle = " << std::endl;
			printVertexData(mesh[n * 3]);
			printVertexData(mesh[n * 3 + 1]);
			printVertexData(mesh[n * 3 + 2]);
		}*/
	}
	void Bind() {
		VAO.Gen();
		VAO.Bind();
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
		mesh.clear();
		glEnableVertexAttribArray(0);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, (void*)0);
		glBindVertexArray(0);
	}

	// Draw the chunk 
	void Draw(Shader &shader) {
		VAO.Bind();
		shader.setVec2("chunkPos", (float)(posx) * CHUNK_SIZE, (float)(posz) * CHUNK_SIZE);
		glDrawArrays(GL_TRIANGLES, 0, verticesNumber);
	}

private:
	inline int getHeight(Noise& noise, int x, int z) {
			return (unsigned)abs(60 + 100.0f * (
			noise.noise(x * (1.0f / 300.0f), z * (1.0f / 300.0f)) * 0.8 +
			noise.noise(x * (1.0f / 150.0f), z * (1.0f / 150.0f)) * 0.5 +
			noise.noise(x * (1.0f / 75.0f), z * (1.0f /75.0f)) * 0.25 /* +
			noise.noise(x * (1.0f / 35.0f), z * (1.0f / 35.0f)) * 0.125 +
			noise.noise(x * (1.0f / 17.5f), z * (1.0f / 17.5f)) * 0.0625*/
		));
	}

	inline void addTopVertices(const int y, const int x, const int z) {
		unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(1,0);

		// v.insert(v.end(), std::begin(a), std::end(a)); instead of multiples push_back
		mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
		mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 0));

		mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
		mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
	}

	inline void addBottomVertices(const int y, const int x, const int z) {
		unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(2, 0);

		mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, z, 0, 0));
		mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
		mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));

		mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
		mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
		mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 1));
	}

	inline void addFrontVertices(const int y, const int x, const int z) {
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));


		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 1));
	}

	inline void addBackVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));

		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
	}

	inline void addRightVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 0, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));

		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
	}

	inline void addLeftVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 1, 0));

		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 1, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 1, 1));
	}

	inline void addVisibleVertices(t_cubes cubes, int x, int y, int z) {

		if (y == 255 || cubes[y + 1][x][z] == AIR)
			addTopVertices(y, x, z);
		if (y > 0 && cubes[y - 1][x][z] == AIR)
			addBottomVertices(y, x, z);

		if (x == CHUNK_SIZE - 1 || z == CHUNK_SIZE - 1)
			return;

		if (x > 0 && cubes[y][x - 1][z] == AIR)
			addFrontVertices(y, x, z);
		if (cubes[y][x + 1][z] == AIR)
			addBackVertices(y, x, z);
		if (z > 0 && cubes[y][x][z - 1] == AIR)
			addLeftVertices(y, x, z);
		if (cubes[y][x][z + 1] == AIR)
			addRightVertices(y, x, z);
	}

	void createMeshData(t_cubes cubes) {

		for (int y = 0; y < 255; y++)
			for (int x = 0; x < CHUNK_SIZE; x++)
				for (int z = 0; z < CHUNK_SIZE; z++)
					if (cubes[y][x][z])
						addVisibleVertices(cubes, x, y, z);
	}

};

#endif