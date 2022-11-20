#include "chunk.h"
#include "blocks.h"

extern Block blocks[256];

void Chunk::addVisibleVertices(int x, int y, int z) {

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

void Chunk::addVisibleBorderVertices() {
	for (int y = 0; y < 255; y++)
		for (int x = 0; x < CHUNK_SIZE; x++) { // change neighbour from vector to chunk** because [] operator take time
			if (cubes[GET_CUBE(0, y, x)] && neighbour[CHUNK_FRONT_SIDE] && neighbour[CHUNK_FRONT_SIDE]->cubes[GET_CUBE(15, y, x)] == AIR) 
				addFrontVertices(0, y, x);
			if (cubes[GET_CUBE(15, y, x)] && neighbour[CHUNK_BACK_SIDE] && neighbour[CHUNK_BACK_SIDE]->cubes[GET_CUBE(0, y, x)] == AIR)
				addBackVertices(15, y, x);
			if (cubes[GET_CUBE(x, y, 15)] && neighbour[CHUNK_RIGHT_SIDE] && neighbour[CHUNK_RIGHT_SIDE]->cubes[GET_CUBE(x, y, 0)] == AIR)
				addRightVertices(x, y, 15);
			if (cubes[GET_CUBE(x, y, 0)] && neighbour[CHUNK_LEFT_SIDE] && neighbour[CHUNK_LEFT_SIDE]->cubes[GET_CUBE(x, y, 15)] == AIR)
				addLeftVertices(x, y, 0);
		}
	verticesNumber = (unsigned int)mesh.size();
}

inline int getVertexAO(int side1, int side2, int corner) {
	if (side1 && side2)
		return (3);
	return ((side1 + side2 + corner));
}

void Chunk::getSideAO(int x, int y, int z, int* result, int pivot) {
	char visibleCubes[3][3];
	int* val1;
	int* val2;

	val1 = &x;
	val2 = &z;
	if (pivot == 0) // the pivot is x
		val1 = &y;
	else if (pivot == 2) // the pivot is z
		val2 = &y;
	*val1 -= 1;
	*val2 -= 1;
	for (int n = 0; n < 3; n++) {
		for (int m = 0; m < 3; m++) {	
			visibleCubes[n][m] = (GetCube(x, y, z) != AIR);
			*val2 += 1;
		}
		*val1 += 1;
		*val2 -= 3;
	}
	result[0] = getVertexAO(visibleCubes[1][0], visibleCubes[0][1], visibleCubes[0][0]);
	result[1] = getVertexAO(visibleCubes[1][2], visibleCubes[0][1], visibleCubes[0][2]);
	result[2] = getVertexAO(visibleCubes[2][1], visibleCubes[1][0], visibleCubes[2][0]);
	result[3] = getVertexAO(visibleCubes[2][1], visibleCubes[1][2], visibleCubes[2][2]);
}

inline void fillQuad(unsigned int *vertices, unsigned int *result) {
	//first triangle
	result[0] = vertices[0];
	result[1] = vertices[1];
	result[2] = vertices[2];

	//second triangle
	result[3] = vertices[2];
	result[4] = vertices[1];
	result[5] = vertices[3];
}

inline void fillFlippedQuad(unsigned int* vertices, unsigned int* result) {
	//first triangle
	result[0] = vertices[0];
	result[1] = vertices[1];
	result[2] = vertices[3];

	//second triangle
	result[3] = vertices[0];
	result[4] = vertices[3];
	result[5] = vertices[2];
}

void Chunk::addTopVertices(const int x, const int y, const int z) {
	unsigned int textureData = PACK_TEXTURE_ID(blocks[cubes[GET_CUBE(x, y, z)]].top);
	int sideAO[4];
	unsigned int vertices[4];

	getSideAO(x, y + 1, z, sideAO, 1);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | textureData | PACK_VERTEX_DATA( x,		(y + 1),  z,		0, 0);
	vertices[1] = sideAO[1] | textureData | PACK_VERTEX_DATA( x,		(y + 1), (z + 1),	1, 0);
	vertices[2] = sideAO[2] | textureData | PACK_VERTEX_DATA((x + 1), (y + 1),  z,		0, 1);
	vertices[3] = sideAO[3] | textureData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1),	1, 1);
	
	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addBottomVertices(const int x, const int y, const int z) {
	unsigned int textureData = PACK_TEXTURE_ID(blocks[cubes[GET_CUBE(x, y, z)]].bottom);
	int sideAO[4];
	unsigned int vertices[4];

	getSideAO(x, y - 1, z, sideAO, 1);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | textureData | PACK_VERTEX_DATA(x, y, z, 0, 0);
	vertices[2] = sideAO[1] | textureData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0);
	vertices[1] = sideAO[2] | textureData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1);
	vertices[3] = sideAO[3] | textureData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 1);

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addFrontVertices(const int x, const int y, const int z) {
	unsigned int textureData = PACK_TEXTURE_ID(blocks[cubes[GET_CUBE(x, y, z)]].side);
	int sideAO[4];
	unsigned int vertices[4];

	getSideAO(x - 1, y, z, sideAO, 0);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | textureData | PACK_VERTEX_DATA(x, y, z, 0, 0);
	vertices[1] = sideAO[1] | textureData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0);
	vertices[2] = sideAO[2] | textureData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 1);
	vertices[3] = sideAO[3] | textureData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 1);

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addBackVertices(const int x, const int y, const int z) {
	unsigned int textureData = PACK_TEXTURE_ID(blocks[cubes[GET_CUBE(x, y, z)]].side);
	int sideAO[4];
	unsigned int vertices[4];

	getSideAO(x + 1, y, z, sideAO, 0);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | textureData | PACK_VERTEX_DATA((x + 1), y, z, 0, 0);
	vertices[2] = sideAO[1] | textureData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0);
	vertices[1] = sideAO[2] | textureData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1);
	vertices[3] = sideAO[3] | textureData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1);

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addRightVertices(const int x, const int y, const int z) {
	unsigned int textureData = PACK_TEXTURE_ID(blocks[cubes[GET_CUBE(x, y, z)]].side);
	int sideAO[4];
	unsigned int vertices[4];

	getSideAO(x, y, z + 1, sideAO, 2);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | textureData | PACK_VERTEX_DATA(x, y, (z + 1), 0, 0);
	vertices[2] = sideAO[1] | textureData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1);
	vertices[1] = sideAO[2] | textureData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0);
	vertices[3] = sideAO[3] | textureData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1);

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addLeftVertices(const int x, const int y, const int z) {
	unsigned int textureData = PACK_TEXTURE_ID(blocks[cubes[GET_CUBE(x, y, z)]].side);
	int sideAO[4];
	unsigned int vertices[4];

	getSideAO(x, y, z - 1, sideAO, 2);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | textureData | PACK_VERTEX_DATA(x, y, z, 0, 0);
	vertices[1] = sideAO[1] | textureData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 1);
	vertices[2] = sideAO[2] | textureData | PACK_VERTEX_DATA((x + 1), y, z, 1, 0);
	vertices[3] = sideAO[3] | textureData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 1, 1);

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}
