#include "chunk.h"

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

void Chunk::addVisibleBorderVertices(char sides) {
	for (int y = 0; y < 255; y++)
		for (int x = 0; x < CHUNK_SIZE; x++) {
			if (((sides >> CHUNK_FRONT_SIDE) & 1) && cubes[GET_CUBE(0, y, x)] && neighbour[CHUNK_FRONT_SIDE]->cubes[GET_CUBE(15, y, x)] == AIR)
				addFrontVertices(0, y, x);
			if (((sides >> CHUNK_BACK_SIDE) & 1) && cubes[GET_CUBE(15, y, x)] && neighbour[CHUNK_BACK_SIDE]->cubes[GET_CUBE(0, y, x)] == AIR)
				addBackVertices(15, y, x);
			if (((sides >> CHUNK_RIGHT_SIDE) & 1) && cubes[GET_CUBE(x, y, 15)] && neighbour[CHUNK_RIGHT_SIDE]->cubes[GET_CUBE(x, y, 0)] == AIR)
				addRightVertices(x, y, 15);
			if (((sides >> CHUNK_LEFT_SIDE) & 1) && cubes[GET_CUBE(x, y, 0)] && neighbour[CHUNK_LEFT_SIDE]->cubes[GET_CUBE(x, y, 15)] == AIR)
				addLeftVertices(x, y, 0);
		}
	verticesNumber = (unsigned int)mesh.size();
}

void Chunk::addTopVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(blocks[cubes[GET_CUBE(x, y, z)]].top, 0);

	// v.insert(v.end(), std::begin(a), std::end(a)); instead of multiples push_back
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 0));

	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
}

void Chunk::addBottomVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(blocks[cubes[GET_CUBE(x, y, z)]].bottom, 0);

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, z, 0, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 1));
}

void Chunk::addFrontVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(blocks[cubes[GET_CUBE(x, y, z)]].side, 0);

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, z, 0, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));


	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 1));
}

void Chunk::addBackVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(blocks[cubes[GET_CUBE(x, y, z)]].side, 0);

	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 0, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));

	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
}

void Chunk::addRightVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(blocks[cubes[GET_CUBE(x, y, z)]].side, 0);

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, (z + 1), 0, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
}

void Chunk::addLeftVertices(const int x, const int y, const int z) {
	unsigned int atlasData = PACK_ATLAS_VERTEX_DATA(blocks[cubes[GET_CUBE(x, y, z)]].side, 0);

	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, y, z, 0, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 1, 0));

	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), y, z, 1, 0));
	mesh.push_back(atlasData | PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
	mesh.push_back(atlasData | PACK_VERTEX_DATA((x + 1), (y + 1), z, 1, 1));
}
