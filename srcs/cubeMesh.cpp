#include "chunk.h"
#include "blocks.h"

void Chunk::addVisibleVertices(int x, int y, int z) {

	if (y == 255 || cubes[GET_CUBE(x, (y + 1), z)] == AIR)//|| blocks[cubes[GET_CUBE(x, (y + 1), z)]].visibility == VISIBILITY::TRANSPARENT)
		addTopVertices(x, y, z);
	if (y > 0 && (cubes[GET_CUBE(x, (y - 1), z)] == AIR))//|| blocks[cubes[GET_CUBE(x, (y - 1), z)]].visibility == VISIBILITY::TRANSPARENT))
		addBottomVertices(x, y, z);
	if (x > 0 && (cubes[GET_CUBE((x - 1), y, z)] == AIR))//|| blocks[cubes[GET_CUBE((x - 1), y, z)]].visibility == VISIBILITY::TRANSPARENT))
		addFrontVertices(x, y, z);
	if (z > 0 && (cubes[GET_CUBE(x, y, (z - 1))] == AIR))//|| blocks[cubes[GET_CUBE(x, y, (z - 1))]].visibility == VISIBILITY::TRANSPARENT))
		addLeftVertices(x, y, z);
	if (x < CHUNK_SIZE - 1 && (cubes[GET_CUBE((x + 1), y, z)] == AIR))//|| blocks[cubes[GET_CUBE((x + 1), y, z)]].visibility == VISIBILITY::TRANSPARENT))
		addBackVertices(x, y, z);
	if (z < CHUNK_SIZE - 1 && (cubes[GET_CUBE(x, y, (z + 1))] == AIR))//|| blocks[cubes[GET_CUBE(x, y, (z + 1))]].visibility == VISIBILITY::TRANSPARENT))
		addRightVertices(x, y, z);
}

void Chunk::addVisibleBorderVertices() {
	for (int y = 0; y < 255; y++)
		for (int x = 0; x < CHUNK_SIZE; x++) {
			if (cubes[GET_CUBE(0, y, x)] && frontNeighbour && (frontNeighbour->cubes[GET_CUBE(15, y, x)] == AIR))//|| blocks[frontNeighbour->cubes[GET_CUBE(15, y, x)]].visibility == VISIBILITY::TRANSPARENT))
				addFrontVertices(0, y, x);
			if (cubes[GET_CUBE(15, y, x)] && backNeighbour && (backNeighbour->cubes[GET_CUBE(0, y, x)] == AIR))//|| blocks[backNeighbour->cubes[GET_CUBE(0, y, x)]].visibility == VISIBILITY::TRANSPARENT))
				addBackVertices(15, y, x);
			if (cubes[GET_CUBE(x, y, 15)] && rightNeighbour && (rightNeighbour->cubes[GET_CUBE(x, y, 0)] == AIR))//|| blocks[rightNeighbour->cubes[GET_CUBE(x, y, 0)]].visibility == VISIBILITY::TRANSPARENT))
				addRightVertices(x, y, 15);
			if (cubes[GET_CUBE(x, y, 0)] && leftNeighbour && (leftNeighbour->cubes[GET_CUBE(x, y, 15)] == AIR))//|| blocks[leftNeighbour->cubes[GET_CUBE(x, y, 15)]].visibility == VISIBILITY::TRANSPARENT))
				addLeftVertices(x, y, 0);
		}
	verticesNumber = (unsigned int)mesh.size();
}

inline int getVertexAO(int side1, int side2, int corner) {
	if (side1 && side2)
		return (3);
	return ((side1 + side2 + corner));
}

#pragma optimize("g", off) // to prevent the compiler to change the for loop to val1 += 3
void Chunk::getSideAO(int x, int y, int z, int* result, int pivot) {
	char visibleCubes[3][3];
	int* val1;
	int* val2;
	
	val1 = (pivot == 0) ? &y : &x; // the pivot is x
	val2 = (pivot == 2) ? &y : &z; // the pivot is z
	*val1 -= 1;
	*val2 -= 1;
	for (int n = 0; n < 3; n++) {
		for (int m = 0; m < 3; m++) {	
			visibleCubes[n][m] = (char)(GetCube(x, y, z) != AIR);
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
#pragma optimize("", on)

inline void fillQuad(int64_t *vertices, int64_t *result) {
	//first triangle
	result[0] = vertices[0];
	result[1] = vertices[1];
	result[2] = vertices[2];

	//second triangle
	result[3] = vertices[2];
	result[4] = vertices[1];
	result[5] = vertices[3];
}

inline void fillFlippedQuad(int64_t *vertices, int64_t *result) {
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
	int textureID= blocks[cubes[GET_CUBE(x, y, z)]].top;
	int sideAO[4];
	int64_t vertices[4];

	getSideAO(x, y + 1, z, sideAO, 1);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | PACK_VERTEX_POS( x,		(y + 1),  z)		| PACK_VERTEX_DATA(textureID, 0, 0, 0);  
	vertices[1] = sideAO[1] | PACK_VERTEX_POS( x,		(y + 1), (z + 1))	| PACK_VERTEX_DATA(textureID, 1, 0, 0);  
	vertices[2] = sideAO[2] | PACK_VERTEX_POS((x + 1), (y + 1),  z)			| PACK_VERTEX_DATA(textureID, 0, 1, 0);  
	vertices[3] = sideAO[3] | PACK_VERTEX_POS((x + 1), (y + 1), (z + 1))	| PACK_VERTEX_DATA(textureID, 1, 1, 0);  
	
	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addBottomVertices(const int x, const int y, const int z) {
	int textureID = blocks[cubes[GET_CUBE(x, y, z)]].bottom;
	int sideAO[4];
	int64_t vertices[4];

	getSideAO(x, y - 1, z, sideAO, 1);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | PACK_VERTEX_POS(x, y, z) 				| PACK_VERTEX_DATA(textureID, 0, 0, 1);  
	vertices[2] = sideAO[1] | PACK_VERTEX_POS(x, y, (z + 1))		| PACK_VERTEX_DATA(textureID, 1, 0, 1);  
	vertices[1] = sideAO[2] | PACK_VERTEX_POS((x + 1), y, z)		| PACK_VERTEX_DATA(textureID, 0, 1, 1);  
	vertices[3] = sideAO[3] | PACK_VERTEX_POS((x + 1), y, (z + 1))	| PACK_VERTEX_DATA(textureID, 1, 1, 1);  

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addFrontVertices(const int x, const int y, const int z) {
	int textureID = blocks[cubes[GET_CUBE(x, y, z)]].side;
	int sideAO[4];
	int64_t vertices[4];

	getSideAO(x - 1, y, z, sideAO, 0);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | PACK_VERTEX_POS(x, y, z)				| PACK_VERTEX_DATA(textureID, 0, 0, 2);  
	vertices[1] = sideAO[1] | PACK_VERTEX_POS(x, y, (z + 1))		| PACK_VERTEX_DATA(textureID, 1, 0, 2);  
	vertices[2] = sideAO[2] | PACK_VERTEX_POS(x, (y + 1), z)		| PACK_VERTEX_DATA(textureID, 0, 1, 2);  
	vertices[3] = sideAO[3] | PACK_VERTEX_POS(x, (y + 1), (z + 1))	| PACK_VERTEX_DATA(textureID, 1, 1, 2);  

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addBackVertices(const int x, const int y, const int z) {
	int textureID = blocks[cubes[GET_CUBE(x, y, z)]].side;
	int sideAO[4];
	int64_t vertices[4];

	getSideAO(x + 1, y, z, sideAO, 0);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | PACK_VERTEX_POS((x + 1), y, z)				| PACK_VERTEX_DATA(textureID, 0, 0, 3);  
	vertices[2] = sideAO[1] | PACK_VERTEX_POS((x + 1), y, (z + 1))			| PACK_VERTEX_DATA(textureID, 1, 0, 3);  
	vertices[1] = sideAO[2] | PACK_VERTEX_POS((x + 1), (y + 1), z)			| PACK_VERTEX_DATA(textureID, 0, 1, 3);  
	vertices[3] = sideAO[3] | PACK_VERTEX_POS((x + 1), (y + 1), (z + 1))	| PACK_VERTEX_DATA(textureID, 1, 1, 3);  

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addRightVertices(const int x, const int y, const int z) {
	int textureID = blocks[cubes[GET_CUBE(x, y, z)]].side;
	int sideAO[4];
	int64_t vertices[4];

	getSideAO(x, y, z + 1, sideAO, 2);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | PACK_VERTEX_POS(x, y, (z + 1))			 | PACK_VERTEX_DATA(textureID, 0, 0, 4);  
	vertices[2] = sideAO[1] | PACK_VERTEX_POS(x, (y + 1), (z + 1))		 | PACK_VERTEX_DATA(textureID, 0, 1, 4);  
	vertices[1] = sideAO[2] | PACK_VERTEX_POS((x + 1), y, (z + 1))		 | PACK_VERTEX_DATA(textureID, 1, 0, 4);  
	vertices[3] = sideAO[3] | PACK_VERTEX_POS((x + 1), (y + 1), (z + 1)) | PACK_VERTEX_DATA(textureID, 1, 1, 4);  

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}

void Chunk::addLeftVertices(const int x, const int y, const int z) {
	int textureID = blocks[cubes[GET_CUBE(x, y, z)]].side;
	int sideAO[4];
	int64_t vertices[4];

	getSideAO(x, y, z - 1, sideAO, 2);
	for (int n = 0; n < 4; n++)
		sideAO[n] <<= 28;
	vertices[0] = sideAO[0] | PACK_VERTEX_POS(x, y, z)				 | PACK_VERTEX_DATA(textureID, 0, 0, 5);  
	vertices[1] = sideAO[1] | PACK_VERTEX_POS(x, (y + 1), z)		 | PACK_VERTEX_DATA(textureID, 0, 1, 5);  
	vertices[2] = sideAO[2] | PACK_VERTEX_POS((x + 1), y, z)		 | PACK_VERTEX_DATA(textureID, 1, 0, 5);  
	vertices[3] = sideAO[3] | PACK_VERTEX_POS((x + 1), (y + 1), z)	 | PACK_VERTEX_DATA(textureID, 1, 1, 5);  

	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
	else
		fillFlippedQuad(vertices, &mesh[verticeNb]);
}
