#include "chunk.h"
#include "blocks.h"

struct XYZtoXZ {
	union {
		int value;
		struct {
			uint8_t x;
			uint8_t y;
			uint8_t z;
		};
		struct {
			uint8_t first;
			uint8_t pivot;
			uint8_t second;
		};
	};

	void init(uint8_t x, uint8_t y, uint8_t z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct XYZtoXY {
	union {
		int value;
		struct {
			uint8_t x;
			uint8_t y;
			uint8_t z;
		};
		struct {
			uint8_t first;
			uint8_t second;
			uint8_t pivot;
		};
	};

	void init(uint8_t x, uint8_t y, uint8_t z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct XYZtoZY {
	union {
		int value;
		struct {
			uint8_t x;
			uint8_t y;
			uint8_t z;
		};
		struct {
			uint8_t pivot;
			uint8_t second;
			uint8_t first;
		};
	};

	void init(uint8_t x, uint8_t y, uint8_t z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};


#define addTopVertices		addVertices<Block::TOP,		XYZtoXZ, 1, 0, true>
#define addBottomVertices	addVertices<Block::BOTTOM,	XYZtoXZ, 0, 1, false>
#define addFrontVertices	addVertices<Block::SIDE,	XYZtoZY, 0, 2, false>
#define addBackVertices		addVertices<Block::SIDE,	XYZtoZY, 1, 3, true>
#define addRightVertices	addVertices<Block::SIDE,	XYZtoXY, 1, 4, false>
#define addLeftVertices		addVertices<Block::SIDE,	XYZtoXY, 0, 5, true>


void Chunk::addVisibleVertices(int x, int y, int z) {

	if (y == 255 || cubes[GET_CUBE(x, (y + 1), z)] == AIR)
		addTopVertices(x, y, z, glm::ivec2(1, 1));
	if (y > 0 && cubes[GET_CUBE(x, (y - 1), z)] == AIR)
		addBottomVertices(x, y, z, glm::ivec2(1, 1));
	if (x > 0 && cubes[GET_CUBE((x - 1), y, z)] == AIR)
		addFrontVertices(x, y, z, glm::ivec2(1, 1));
	if (z > 0 && cubes[GET_CUBE(x, y, (z - 1))] == AIR)
		addLeftVertices(x, y, z, glm::ivec2(1, 1));
	if (x < CHUNK_SIZE - 1 && cubes[GET_CUBE((x + 1), y, z)] == AIR)
		addBackVertices(x, y, z, glm::ivec2(1, 1));
	if (z < CHUNK_SIZE - 1 && cubes[GET_CUBE(x, y, (z + 1))] == AIR)
		addRightVertices(x, y, z, glm::ivec2(1, 1));
}

template <int Y>
glm::ivec2 Chunk::GreedyMeshingY(std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> &chunkSlice, int x, int y, int z) {
	int sizex, sizez, type;

	if ((cubes[GET_CUBE(x, y, z)] != AIR && !chunkSlice[x][z]) &&
		(y + Y >= 0 && y + Y <= 255 && cubes[GET_CUBE(x, (y + Y), z)] == AIR)) {
		type = cubes[GET_CUBE(x, y, z)];
		chunkSlice[x][z] = 1;
		for (sizex = 1; x + sizex < CHUNK_SIZE; sizex++) {
			if (cubes[GET_CUBE(x + sizex, y, z)] == type && !chunkSlice[x + sizex][z] &&
				(y + Y >= 0 && y + Y <= 255 && cubes[GET_CUBE(x + sizex, (y + Y), z)] == AIR)) { // for top and bottom do y + ? >= 0 && y + ? <= 255
				chunkSlice[x + sizex][z] = 1;
			}
			else
				break;
		}
		for (sizez = 1; z + sizez < CHUNK_SIZE; sizez++) {
			bool valid = true;
			for (int n = 0; n < sizex; n++) {
				if (cubes[GET_CUBE(x + n, y, z + sizez)] != type || chunkSlice[x + n][z + sizez] ||
					(y + Y < 0 || y + Y > 255 || cubes[GET_CUBE(x + n, (y + Y), z + sizez)] != AIR)) {
					valid = false;
					break;
				}
			}
			if (valid) {
				for (int n = 0; n < sizex; n++)
					chunkSlice[x + n][z + sizez] = 1;
			}
			else
				break;
		}
		return glm::ivec2(sizex, sizez);
	}
	return glm::ivec2(0, 0);
}

template <int X>
glm::ivec2 Chunk::GreedyMeshingX(std::array<std::array<uint8_t, 256>, CHUNK_SIZE>& chunkSlice, int x, int y, int z) {
	int sizex, sizez, type;

	if ((cubes[GET_CUBE(x, y, z)] != AIR && !chunkSlice[z][y]) &&
		(x + X >= 0 && x + X < CHUNK_SIZE && cubes[GET_CUBE(x + X, y , z)] == AIR)) {
		type = cubes[GET_CUBE(x, y, z)];
		chunkSlice[z][y] = 1;
		for (sizex = 1; z + sizex < CHUNK_SIZE; sizex++) {
			if (cubes[GET_CUBE(x, y, z + sizex)] == type && !chunkSlice[z + sizex][y] &&
				(x + X >= 0 && x + X < CHUNK_SIZE && cubes[GET_CUBE(x + X, y, z + sizex)] == AIR)) { // for top and bottom do y + ? >= 0 && y + ? <= 255
				chunkSlice[z + sizex][y] = 1;
			}
			else
				break;
		}
		for (sizez = 1; y + sizez < 256; sizez++) {
			bool valid = true;
			for (int n = 0; n < sizex; n++) {
				if (cubes[GET_CUBE(x, y + sizez, z + n)] != type || chunkSlice[z + n][y + sizez] ||
					(x + X < 0 || x + X >= CHUNK_SIZE || cubes[GET_CUBE(x + X, y + sizez, z + n)] != AIR)) {
					valid = false;
					break;
				}
			}
			if (valid) {
				for (int n = 0; n < sizex; n++)
					chunkSlice[z + n][y + sizez] = 1;
			}
			else
				break;
		}
		return glm::ivec2(sizex, sizez);
	}
	return glm::ivec2(0, 0);
}

template <int Z>
glm::ivec2 Chunk::GreedyMeshingZ(std::array<std::array<uint8_t, 256>, CHUNK_SIZE>& chunkSlice, int x, int y, int z) {
	int sizex, sizez, type;

	if (cubes[GET_CUBE(x, y, z)] != AIR && !chunkSlice[x][y] &&
		z + Z >= 0 && z + Z < CHUNK_SIZE && cubes[GET_CUBE(x , y, z + Z)] == AIR) {
		type = cubes[GET_CUBE(x, y, z)];
		chunkSlice[x][y] = 1;
		for (sizex = 1; x + sizex < CHUNK_SIZE; sizex++) {
			if (cubes[GET_CUBE(x + sizex, y, z)] == type && !chunkSlice[x + sizex][y] &&
				(z + Z >= 0 && z + Z < CHUNK_SIZE && cubes[GET_CUBE(x + sizex, y, z + Z)] == AIR)) { // for top and bottom do y + ? >= 0 && y + ? <= 255
				chunkSlice[x + sizex][y] = 1;
			}
			else
				break;
		}
		for (sizez = 1; y + sizez < 256; sizez++) {
			bool valid = true;
			for (int n = 0; n < sizex; n++) {
				if (cubes[GET_CUBE(x + n, y + sizez, z)] != type || chunkSlice[x + n][y + sizez] ||
					(x + Z < 0 || x + Z >= CHUNK_SIZE || cubes[GET_CUBE(x + n, y + sizez, z + Z)] != AIR)) {
					valid = false;
					break;
				}
			}
			if (valid) {
				for (int n = 0; n < sizex; n++)
					chunkSlice[x + n][y + sizez] = 1;
			}
			else
				break;
		}
		return glm::ivec2(sizex, sizez);
	}
	return glm::ivec2(0, 0);
}

// generate a greedy mesh based on the chunk cube data
void Chunk::GreedyMeshALL() {

	std::array<std::array<std::array<uint8_t, 256>, CHUNK_SIZE>, 4>			chunkSlice;
	std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>, 2>	chunkSliceY;
	glm::ivec2 size;

	for (int x = 0; x < CHUNK_SIZE; x++) {
		memset(&chunkSlice, 0, CHUNK_SIZE * 256 * 4);
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < 256; y++) {
				if (cubes[GET_CUBE(x, y, z)] != AIR) {
					if ((size = GreedyMeshingX<1>(chunkSlice[0], x, y, z)).x)
						addBackVertices(x, y, z, size);
					if ((size = GreedyMeshingX<-1>(chunkSlice[1], x, y, z)).x)
						addFrontVertices(x, y, z, size);
				}
				if (cubes[GET_CUBE(z, y, x)] != AIR) {
					if ((size = GreedyMeshingZ<1>(chunkSlice[2], z, y, x)).x)
						addRightVertices(z, y, x, size);
					if ((size = GreedyMeshingZ<-1>(chunkSlice[3], z, y, x)).x)
						addLeftVertices(z, y, x, size);
				}
			}
		}
	}

	for (int y = 0; y < 255; y++) {
		memset(&chunkSliceY, 0, CHUNK_SIZE * CHUNK_SIZE * 2);
		for (int x = 0; x < CHUNK_SIZE; x++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				if (cubes[GET_CUBE(x, y, z)] != AIR) {
					if ((size = GreedyMeshingY<1>(chunkSliceY[0], x, y, z)).x)
						addTopVertices(x, y, z, size);
					if ((size = GreedyMeshingY<-1>(chunkSliceY[1], x, y, z)).x)
						addBottomVertices(x, y, z, size);
				}
			}
		}
	}
}

void Chunk::addVisibleBorderVertices() {
	for (int y = 0; y < 255; y++)
		for (int x = 0; x < CHUNK_SIZE; x++) {
			if (cubes[GET_CUBE(0, y, x)] && frontNeighbour && frontNeighbour->cubes[GET_CUBE(15, y, x)] == AIR)
				addFrontVertices(0, y, x, glm::ivec2(1, 1));
			if (cubes[GET_CUBE(15, y, x)] && backNeighbour && backNeighbour->cubes[GET_CUBE(0, y, x)] == AIR)
				addBackVertices(15, y, x, glm::ivec2(1, 1));
			if (cubes[GET_CUBE(x, y, 15)] && rightNeighbour && rightNeighbour->cubes[GET_CUBE(x, y, 0)] == AIR)
				addRightVertices(x, y, 15, glm::ivec2(1, 1));
			if (cubes[GET_CUBE(x, y, 0)] && leftNeighbour && leftNeighbour->cubes[GET_CUBE(x, y, 15)] == AIR)
				addLeftVertices(x, y, 0, glm::ivec2(1, 1));
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

template <Block::TEXTURE_TYPE TYPE, typename XYZto2D, int OFFSET, int NORMAL, bool CW>
void Chunk::addVertices(const int x, const int y, const int z, const glm::ivec2 size) {
	int textureID= blocks[cubes[GET_CUBE(x, y, z)]].GetTexture(TYPE);
//	int sideAO[4];
	int64_t vertices[4];
	XYZto2D pos[4];

	//getSideAO(x, y + 1, z, sideAO, 1);
	//for (int n = 0; n < 4; n++)
	//	sideAO[n] <<= 28;
	pos[0].init(x, y, z);
	pos[0].pivot += OFFSET;
	pos[1].value = pos[0].value;
	pos[1].second += size.y;
	pos[2].value = pos[0].value;
	pos[2].first += size.x;
	pos[3].value = pos[1].value;
	pos[3].first += size.x;
	vertices[0			] = /*sideAO[0] |*/ PACK_VERTEX_POS(pos[0].x, pos[0].y, pos[0].z)	| PACK_VERTEX_DATA(textureID, 0, 0,				NORMAL);
	vertices[1 + !(CW)	] = /*sideAO[1] |*/ PACK_VERTEX_POS(pos[1].x, pos[1].y, pos[1].z)	| PACK_VERTEX_DATA(textureID, 0, size.y,		NORMAL);
	vertices[1 + (CW)	] = /*sideAO[2] |*/ PACK_VERTEX_POS(pos[2].x, pos[2].y, pos[2].z)	| PACK_VERTEX_DATA(textureID, size.x, 0,		NORMAL);
	vertices[3			] = /*sideAO[3] |*/ PACK_VERTEX_POS(pos[3].x, pos[3].y, pos[3].z)	| PACK_VERTEX_DATA(textureID, size.x, size.y,	NORMAL);
	
	size_t verticeNb = mesh.size();
	mesh.resize(verticeNb + 6);

//	if (sideAO[0] + sideAO[3] > sideAO[1] + sideAO[2])
		fillQuad(vertices, &mesh[verticeNb]);
//	else
//		fillFlippedQuad(vertices, &mesh[verticeNb]);
}