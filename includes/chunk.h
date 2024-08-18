#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include <glad/glad.h>
#include <VAO.h>
#include <vector>
#include <unordered_map>
#include "shader.h"
#include "blocks.h"
#include "generation.h"
#include "global_defines.h"

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


#define CHUNK_NOT_PROCESSING 0
#define CHUNK_PROCESSING 1 << 4


/*
	store multiple data in an int32 with the format :

	vertex pos :
	0000		|	0000000000	|	00000	|	00000	|	00000000
	luminosity	|	 		 	|	posZ	|	posX	|	  posY

	vertex data :
	0000000000000	|	0000	|	0000	|	0000	|	00000000
			 		|  normal	|	textY	|	textX	|	 textID
*/
#define PACK_VERTEX_POS(x, y, z) ((y) | ((x) << 8) | ((z) << 13))
#define PACK_VERTEX_DATA(textID, textx, texty, normal) ((int64_t)((textID) | (textx) <<  8 | (texty) << 12 | (normal) << 16) << 32)

// get the offset of a cube based on the position in the chunk
#define GET_CUBE(x, y, z) (((y) << 8) | ((x) << 4) | (z))

// typedef unsigned char t_cubes[256][CHUNK_SIZE][CHUNK_SIZE];

#define GET_CHUNK_ID(x, z) ((int64_t)(x) << 32 | ((z) & UINT32_MAX))

#define VEC2_LEN(x, y) (sqrt((x) * (x) + (y) * (y)))

class Chunk
{
private:

	//the mesh is an array containing vertex position and other information in an unsigned int
	std::vector<int64_t> mesh;
	// the number of all the vertices
	unsigned int verticesNumber;

	// vertex array object
	VAO vao;
	// vertex buffer object ID
	GLuint VBO;

public:
	static Block blocks[MAX_BLOCK_NB];

	static std::unordered_map<int64_t, Chunk*> chunksMap;

	static ChunkGeneration globalChunkGeneration;

	// the visibility of the chunk
	bool isVisible;
	// chunk proximity to the player
	unsigned int playerProximity;
	// pointer to every neighbour of the chunk
	Chunk** neighbour;

	// pointer to each neighbours
	Chunk* frontNeighbour;
	Chunk* backNeighbour;
	Chunk* rightNeighbour;
	Chunk* leftNeighbour;

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
	Chunk();

	// Destructor
	~Chunk();

	// Set the position of the chunk
	void SetPosistion(int x, int z);

	// get all the neighbours of the chunk
	void GetNeighbour();

	// Generate the chunk cubes data
	void Generate();

	// generate a mesh based on the chunk cube data 
	void createMeshData();

	// generate VAO VBO, fill the VBO and bind it to the VAO
	void Bind();

	// Draw the chunk 
	void Draw(Shader& shader) {
		if (vao.ID != 0) {
			vao.Bind();
			shader.setVec2("chunkPos", (float)(posx << 4), (float)(posz << 4));
			glDrawArrays(GL_TRIANGLES, 0, verticesNumber);
		}
	}

	// Draw the chunk if visible
	void DrawVisible(Shader& shader) {
		if (isVisible && vao.ID != 0) {
			vao.Bind();
			shader.setVec2("chunkPos", (float)(posx << 4), (float)(posz << 4));
			McDrawArrays(GL_TRIANGLES, 0, verticesNumber);
		}
	}

	//reload the chunk by recreating the mesh and changing the VBO
	void Update();

	//reload everything nessesary to update a cube
	void UpdateCube(int x, int z);

	//get the id of the cube at a certain position from the chunk
	unsigned char GetCube(int x, int y, int z);
	//get the id of the cube at a certain position in the chunk
	unsigned char GetCube(glm::ivec3 pos);
	//set the id of the cube at a certain position
	void SetCube(unsigned char cubeId, int x, int y, int z);
	// return true if the chunk is surounded by neighbour with data
	bool HasAllNeighbours();
	// lock all the neighbours of the chunk
	void LockNeighbours();
	// unlock all the neighbours of the chunk
	void UnlockNeighbours();

	// set player proximity
	void SetPlayerProximity(const glm::vec3 &playerPos);
private:
	void addTopVertices(const int x, const int y, const int z);
	void addBottomVertices(const int x, const int y, const int z);
	void addFrontVertices(const int x, const int y, const int z);
	void addBackVertices(const int x, const int y, const int z);
	void addRightVertices(const int x, const int y, const int z);
	void addLeftVertices(const int x, const int y, const int z);

	void addGrassVertices(const int x, const int y, const int z);

	//store in result the AO value of each corner of a side
	void getSideAO(int x, int y, int z, int *result, int pivot);

	void addVisibleVertices(int x, int y, int z);
	void addVisibleBorderVertices();
};

inline bool Chunk::HasAllNeighbours() { // instead incement a value when retreving a chunk data from thread
	for (int n = 0; n < 4; n++)
		if (!neighbour[n] || neighbour[n]->status < CHUNK_DATA_LOADED)
			return (false);
	if (!neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_LEFT_SIDE] || neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_LEFT_SIDE]->status < CHUNK_DATA_LOADED ||
		!neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_RIGHT_SIDE] || neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->status < CHUNK_DATA_LOADED || 
		!neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_LEFT_SIDE] || neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_LEFT_SIDE]->status < CHUNK_DATA_LOADED ||
		!neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_RIGHT_SIDE] || neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->status < CHUNK_DATA_LOADED)
		return (false);
	return (true);
}

inline void Chunk::LockNeighbours() {
	for (int n = 0; n < 4; n++)
		neighbour[n]->threadStatus += 1;
	neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_LEFT_SIDE]->threadStatus += 1;
	neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->threadStatus += 1;
	neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_LEFT_SIDE]->threadStatus += 1;
	neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->threadStatus += 1;

}

inline void Chunk::UnlockNeighbours() {
	for (int n = 0; n < 4; n++)
		neighbour[n]->threadStatus -= 1;
	neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_LEFT_SIDE]->threadStatus -= 1;
	neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->threadStatus -= 1;
	neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_LEFT_SIDE]->threadStatus -= 1;
	neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->threadStatus -= 1;
}

// return a pointer to a chunk if it exist based on its coordonate and return 0 if it is not found
Chunk* GetChunk(int x, int z);

// return a the id of a cube based on its world coordonate 
inline unsigned char GetCubeAt(int x, int y, int z) { //peux �tre couteux si appeler trop de fois
	Chunk* chunk;

	chunk = GetChunk(x >> 4, z >> 4);										//faire un get cube relatif a un chunk pour �vit� trop d'appel a GetChunk
	if (!chunk || chunk->status < CHUNK_DATA_LOADED || y < 0 || y > 255)
		return (0);
	return (chunk->cubes[GET_CUBE((x & 0xF), y, (z & 0xF))]);
}

inline unsigned char Chunk::GetCube(int x, int y, int z) {	//faire un getCube qui peux sortire des limites 0 a 15
	Chunk* chunk;

	if (y < 0 || y > 255)
		return 0;
	chunk = this;
	while (x < 0) {
		x += 16;
		if (!(chunk = chunk->neighbour[CHUNK_FRONT_SIDE]))
			return 0;
	}
	while (x > 15) {
		x -= 16;
		if (!(chunk = chunk->neighbour[CHUNK_BACK_SIDE]))
			return 0;
	}
	
	while (z < 0) {
		z += 16;
		if (!(chunk = chunk->neighbour[CHUNK_LEFT_SIDE]))
			return 0;
	}
	while (z > 15) {
		z -= 16;
		if (!(chunk = chunk->neighbour[CHUNK_RIGHT_SIDE]))
			return 0;
	}
	if (chunk->status < CHUNK_DATA_LOADED)
		return 0;
	return (chunk->cubes[GET_CUBE(x, y, z)]);
}

inline unsigned char Chunk::GetCube(glm::ivec3 pos) {
	return (cubes[GET_CUBE(pos.x, pos.y, pos.z)]);
}

inline void Chunk::SetCube(unsigned char cubeId, int x, int y, int z) {
	cubes[GET_CUBE(x, y, z)] = cubeId;
}

#endif