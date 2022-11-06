#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include <glad/glad.h>
#include <VAO.h>
#include <noise.h>
#include <vector>
#include <map>
#include "chunk_generation.h"
#include "blocks.h"
#include "shader.h"

extern Block blocks[256];

// id of air
#define AIR 0
#define CHUNK_SIZE 16

// the id of the chunk sides
#define CHUNK_FRONT_SIDE 0				// maybe do a binary mask to be able to do multiple side at a time
#define CHUNK_BACK_SIDE 1
#define CHUNK_RIGHT_SIDE 2
#define CHUNK_LEFT_SIDE 3

// neighbour loaded status
#define CHUNK_NONE 0
#define CHUNK_ALL_LOADED 0xF

// all the possible status of a chunk

#define CHUNK_UNLOADED 0
// CHUNK_DATA_LOADED the chunk cube data is loaded
#define CHUNK_DATA_LOADED 1
// CHUNK_LOADED means that the chunk mesh is done but incomplete 
#define CHUNK_LOADED 2


#define CHUNK_NOT_PROCESSING 0
#define CHUNK_PROCESSING 1 << 4


/*
	store multiple data in an int32 with the format :

	0000	|	0000	|	0000	|	0	|	0	|	00000	|	00000	|	00000000
	unused	| texatlasY	| texatlasX	| textY	| textX	|	posZ	|	posX	|	  posY
*/
#define PACK_VERTEX_DATA(x, y, z, textx, texty) (y | (x << 8) | (z << 13) | textx << 18 | texty << 19)
#define PACK_ATLAS_VERTEX_DATA(texatlasX, texatlasY) ((texatlasX << 20) | (texatlasY << 24))

// get the offset of a cube based on the position in the chunk
#define GET_CUBE(x, y, z) ((y << 8) | (x << 4) | z)

// typedef unsigned char t_cubes[256][CHUNK_SIZE][CHUNK_SIZE];

#define GET_CHUNK_ID(x, z) ((int64_t)x << 32 | (z & UINT32_MAX))

class Chunk;

extern std::map<int64_t, Chunk*> chunksMap;

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
	// the visibility of the chunk
	bool isVisible;
	// pointer to every neighbour of the chunk
	std::vector<Chunk*> neighbour;			//remove the vector if needed
	// loading status of the chunk
	char status;
	//the thread status of the chunk 
	char threadStatus;				// maybe change the way status are made
	//neighbour status of the chunk
	char neighbourLoaded;
	// position x of the chunk
	int posx;
	// position z of the chunk
	int posz;

	// ids of all the cube in the chunk
	unsigned char *cubes;





	// Default constructor
	Chunk();

	// Destructor
	~Chunk();

	// Set the position of the chunk
	void SetPosistion(int x, int z);

	// return a pointer to a chunk if it exist based on its coordonate and return 0 if it is not found
	Chunk* GetChunk(int x, int z);

	// get all the neighbours of the chunk
	void GetNeighbour();

	// Generate the chunk cubes data
	void Generate();

	// generate a mesh based on the chunk cube data 
	void createMeshData();

	// generate VAO VBO, fill the VBO and bind it to the VAO
	void Bind();

	// Draw the chunk 
	void Draw(Shader& shader);

	//add the mesh between a chunk and its neighbours
	void addNeighbours();

private:
	void addTopVertices(const int y, const int x, const int z);
	void addBottomVertices(const int y, const int x, const int z);
	void addFrontVertices(const int y, const int x, const int z);
	void addBackVertices(const int y, const int x, const int z);
	void addRightVertices(const int y, const int x, const int z);
	void addLeftVertices(const int y, const int x, const int z);

	void addVisibleVertices(int x, int y, int z);
	void addVisibleBorderVertices(char sides);
};


#endif