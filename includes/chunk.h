#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include<glad/glad.h>
#include <VAO.h>
#include <noise.h>
#include<vector>
#include<map>


// id of air
#define AIR 0
#define CHUNK_SIZE 16

// the id of the chunk sides
#define CHUNK_FRONT_SIDE 0				// maybe do a binary mask to be able to do multiple side at a time
#define CHUNK_BACK_SIDE 1
#define CHUNK_RIGHT_SIDE 2
#define CHUNK_LEFT_SIDE 3

// all the possible status of a chunk

#define CHUNK_UNLOADED 0
// CHUNK_DATA_LOADED the chunk cube data is loaded
#define CHUNK_DATA_LOADED 1
// CHUNK_LOADED means that the chunk mesh is done but incomplete 
#define CHUNK_LOADED 2
// CHUNK_FULLY_LOADED means that the chunk mesh is complete
#define CHUNK_FULLY_LOADED 3


#define CHUNK_NOT_PROCESSING 0
#define CHUNK_PROCESSING 1


/*
	store multiple data in an int32 with the format :

	0000	|	0000	|	0000	|	0	|	0	|	00000	|	00000	|	00000000
	unused	| texatlasY	| texatlasX	| textY	| textX	|	posZ	|	posX	|	  posY
*/
#define PACK_VERTEX_DATA(x, y, z, textx, texty) (y | (x << 8) | (z << 13) | textx << 18 | texty << 19)
#define PACK_ATLAS_VERTEX_DATA(texatlasX, texatlasY) ((texatlasX << 20) | (texatlasY << 24))

// get the offset of a cube based on the position in the chunk
#define GET_CUBE(x, y, z) ((y << 8) | (x << 4) | z)

typedef unsigned char t_cubes[256][CHUNK_SIZE][CHUNK_SIZE];

#define GET_CHUNK_ID(x, z) ((int64_t)((int64_t)x << 32 | z))

class Chunk;

std::map<int64_t, Chunk*> chunksMap;

class Chunk
{
private:

	//the mesh is an array containing vertex position and other information in an unsigned int
	std::vector<unsigned int> mesh;
	// the number of all the vertices
	unsigned int verticesNumber;

	// vertex array object
	VAO VAO;
	// vertex buffer object ID
	GLuint VBO;

public:
	// pointer to every neighbour of the chunk
	std::vector<Chunk*> neighbour;			//remove the vector if needed
	// loading status of the chunk
	char status;
	//the thread status of the chunk 
	char threadStatus;				// maybe change the way status are made
	// position x of the chunk
	int posx;
	// position z of the chunk
	int posz;

	// ids of all the cube in the chunk
	unsigned char *cubes;

	// Default constructor
	Chunk() {
	//	std::cout << "constructor called" << std::endl;
		verticesNumber = 0;
		posx = 0;
		posz = 0;
		cubes = 0;
		VBO = 0;
		status = CHUNK_UNLOADED;
		threadStatus = CHUNK_NOT_PROCESSING;
		neighbour.resize(4);
	};

	// Destructor
	~Chunk() {
	//	std::cout << "chunk has been destroyed" << std::endl;
	//	std::cout << "destructor called  and addr = " << cubes << "  x = " << posx << "  z = " << posz << std::endl;
		chunksMap.erase(GET_CHUNK_ID(posx, posz));
		/*if (neighbour[CHUNK_FRONT_SIDE])
			neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_BACK_SIDE] = 0;
		if (neighbour[CHUNK_BACK_SIDE])
			neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_FRONT_SIDE] = 0;
		if (neighbour[CHUNK_RIGHT_SIDE])
			neighbour[CHUNK_RIGHT_SIDE]->neighbour[CHUNK_LEFT_SIDE] = 0;
		if (neighbour[CHUNK_LEFT_SIDE])
			neighbour[CHUNK_LEFT_SIDE]->neighbour[CHUNK_RIGHT_SIDE] = 0;*/
		free(cubes);
		glDeleteBuffers(1, &VBO);
	}

	// Set the position of the chunk
	void SetPosistion(int x, int z) {
	//	std::cout << "SetPosistion called  and addr = " << cubes << "x = " << posx << "z = " << posz << std::endl;

		posx = x;
		posz = z;
		
		/*if ((neighbour[CHUNK_FRONT_SIDE] = chunksMap[(size_t)(x - 1) << 32 | z]))
			neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_BACK_SIDE] = this;
		std::cout << "front key = " << (((size_t)(x - 1) << 32) + (size_t)z) << std::endl;
		if (neighbour[CHUNK_FRONT_SIDE])
			std::cout << " VAO = " << neighbour[CHUNK_FRONT_SIDE]->VAO.ID << std::endl;
		
		if ((neighbour[CHUNK_BACK_SIDE] = chunksMap[(size_t)(x + 1) << 32 | z]))
			neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_FRONT_SIDE] = this;
		std::cout << "back key = " << (((size_t)(x + 1) << 32) + (size_t)z) << std::endl ;
		if (neighbour[CHUNK_BACK_SIDE])
			std::cout << " VAO = " << neighbour[CHUNK_BACK_SIDE]->VAO.ID << std::endl ;
		std::cout << std::endl;
		if ((neighbour[CHUNK_RIGHT_SIDE] = chunksMap[(size_t)x << 32 | (z + 1)]))
			neighbour[CHUNK_RIGHT_SIDE]->neighbour[CHUNK_LEFT_SIDE] = this;
		if ((neighbour[CHUNK_LEFT_SIDE] = chunksMap[(size_t)x << 32 | (z - 1)]))
			neighbour[CHUNK_LEFT_SIDE]->neighbour[CHUNK_RIGHT_SIDE] = this;*/
		neighbour[CHUNK_FRONT_SIDE] = chunksMap[GET_CHUNK_ID((x - 1), z)];
		std::cout << "front key = " << GET_CHUNK_ID((x - 1), z) << std::endl;
		if (neighbour[CHUNK_FRONT_SIDE])
			std::cout << " VAO = " << neighbour[CHUNK_FRONT_SIDE]->VAO.ID << std::endl;
		neighbour[CHUNK_BACK_SIDE] = chunksMap[GET_CHUNK_ID((x + 1), z)];
		std::cout << "back key = " << GET_CHUNK_ID((x + 1), z) << std::endl;
		if (neighbour[CHUNK_BACK_SIDE])
			std::cout << " VAO = " << neighbour[CHUNK_BACK_SIDE]->VAO.ID << std::endl;
		std::cout << std::endl;
		neighbour[CHUNK_RIGHT_SIDE] = chunksMap[GET_CHUNK_ID(x, (z - 1))];
		neighbour[CHUNK_LEFT_SIDE] = chunksMap[GET_CHUNK_ID(x, (z + 1))];

	}

	// Generate the chunk cubes data
	void Generate() {
		if (!(cubes = (unsigned char*)calloc(1, sizeof(unsigned char) * 256 * CHUNK_SIZE * CHUNK_SIZE)))
			return ;

		for (unsigned x = 0; x < CHUNK_SIZE; x++) {
			for (unsigned z = 0; z < CHUNK_SIZE; z++) {
				unsigned height = getHeight(global_noise, posx * CHUNK_SIZE + x, posz * CHUNK_SIZE + z);
				for (unsigned y = 0; y < 256; y++)
					if (y < height)
						cubes[GET_CUBE(x, y, z)] = 1;
			}
		}
		status = CHUNK_DATA_LOADED;
	}

	void createMeshData() {

		mesh.resize(0);
		for (int y = 0; y < 255; y++)
			for (int x = 0; x < CHUNK_SIZE; x++)
				for (int z = 0; z < CHUNK_SIZE; z++)
					if (cubes[GET_CUBE(x, y, z)])
						addVisibleVertices(x, y, z);
		status = CHUNK_LOADED;
		verticesNumber = (unsigned int)mesh.size();
	}

	// generate VAO VBO, fill the VBO and bind it to the VAO
	void Bind() {
		if (!verticesNumber)
			return ;
		VAO.Gen();
		VAO.Bind();
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, (void*)0);
		glBindVertexArray(0);
		threadStatus = CHUNK_NOT_PROCESSING;
	}

	// Draw the chunk 
	inline void Draw(Shader &shader) {
		VAO.Bind();
		shader.setVec2("chunkPos", (float)(posx << 4), (float)(posz << 4));
		glDrawArrays(GL_TRIANGLES, 0, verticesNumber);
	}

	void addNeighbour(Chunk *chunk, int side) {

		neighbour[side] = chunk;
		addVisibleBorderVertices(side);
		if (verticesNumber) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
		}
		if (neighbour[CHUNK_FRONT_SIDE] && neighbour[CHUNK_BACK_SIDE]
			&& neighbour[CHUNK_RIGHT_SIDE] && neighbour[CHUNK_LEFT_SIDE]) {
			mesh.clear();
			status = CHUNK_FULLY_LOADED;
		}
	}

private:
	inline int getHeight(Noise& noise, int x, int z) {
			return (unsigned)abs(30 + 40.0f * (
			noise.noise(x * (1.0f / 300.0f), z * (1.0f / 300.0f)) * 0.8 +
			noise.noise(x * (1.0f / 150.0f), z * (1.0f / 150.0f)) * 0.5 +
			noise.noise(x * (1.0f / 75.0f), z * (1.0f /75.0f)) * 0.25  +
			noise.noise(x * (1.0f / 35.0f), z * (1.0f / 35.0f)) * 0.125 +
			noise.noise(x * (1.0f / 17.5f), z * (1.0f / 17.5f)) * 0.0625
		));
	}

	inline void addTopVertices(const int y, const int x, const int z);
	inline void addBottomVertices(const int y, const int x, const int z);
	inline void addFrontVertices(const int y, const int x, const int z);
	inline void addBackVertices(const int y, const int x, const int z);
	inline void addRightVertices(const int y, const int x, const int z);
	inline void addLeftVertices(const int y, const int x, const int z);

	inline void addVisibleVertices(int x, int y, int z) {

		if (y == 255 || cubes[GET_CUBE(x, (y + 1), z)] == AIR)
			addTopVertices(x, y, z);
		if (y > 0 && cubes[GET_CUBE(x, (y - 1), z)] == AIR)
			addBottomVertices(x, y, z);
		if (x > 0 && cubes[GET_CUBE((x - 1), y, z)] == AIR)
			addFrontVertices(x, y, z);
		if (z > 0 && cubes[GET_CUBE(x, y, (z - 1))] == AIR)
			addLeftVertices(x, y, z);
		if (x < CHUNK_SIZE - 1 && cubes[GET_CUBE((x + 1), y, z)] == AIR)
			addBackVertices(x, y, z);
		if (z < CHUNK_SIZE - 1 && cubes[GET_CUBE(x, y, (z + 1))] == AIR)
			addRightVertices(x, y, z);
	}

	inline void addVisibleBorderVertices(int side) {
		for (int y = 0; y < 255; y++)
			for (int x = 0; x < CHUNK_SIZE; x++) {
				if (side == CHUNK_FRONT_SIDE && cubes[GET_CUBE(0, y, x)] && neighbour[CHUNK_FRONT_SIDE]->cubes[GET_CUBE(15, y, x)] == AIR)
					addFrontVertices(0, y, x);
				if (side == CHUNK_BACK_SIDE && cubes[GET_CUBE(15, y, x)] && neighbour[CHUNK_BACK_SIDE]->cubes[GET_CUBE(0, y, x)] == AIR)
					addBackVertices(15, y, x);
				if (side == CHUNK_RIGHT_SIDE && cubes[GET_CUBE(x, y, 15)] && neighbour[CHUNK_RIGHT_SIDE]->cubes[GET_CUBE(x, y, 0)] == AIR)
					addRightVertices(x, y, 15);
				if (side == CHUNK_LEFT_SIDE && cubes[GET_CUBE(x, y, 0)] && neighbour[CHUNK_LEFT_SIDE]->cubes[GET_CUBE(x, y, 15)] == AIR)
					addLeftVertices(x, y, 0);
			}
		verticesNumber = (unsigned int)mesh.size();
	}	
};

inline void Chunk::addTopVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(1, 0);

	// v.insert(v.end(), std::begin(a), std::end(a)); instead of multiples push_back
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 0));

	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
}

inline void Chunk::addBottomVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(2, 0);

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, z, 0, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 1));
}

inline void Chunk::addFrontVertices(const int x, const int y, const int z) {
	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
	mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));


	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 1));
}

inline void Chunk::addBackVertices(const int x, const int y, const int z) {

	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 0, 0));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));

	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
}

inline void Chunk::addRightVertices(const int x, const int y, const int z) {

	mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 0, 0));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));

	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
}

inline void Chunk::addLeftVertices(const int x, const int y, const int z) {

	mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 1, 0));

	mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 1, 0));
	mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 1, 1));
}

#endif