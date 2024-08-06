#include "chunk.h"
#include "generation.h"

Block Chunk::blocks[MAX_BLOCK_NB];

std::unordered_map<int64_t, Chunk*> Chunk::chunksMap;

ChunkGeneration Chunk::globalChunkGeneration;


// Default constructor
Chunk::Chunk() {
	//	std::cout << "constructor called" << std::endl;
	isVisible = false;
	verticesNumber = 0;
	posx = 0;
	posz = 0;
	cubes = 0;
	VBO = 0;
	status = CHUNK_UNLOADED;
	threadStatus = CHUNK_NOT_PROCESSING;
	neighbour = &frontNeighbour;
	memset(neighbour, 0, 4 * sizeof(Chunk*));
};

// Destructor
Chunk::~Chunk() {
	//	std::cout << "chunk has been destroyed" << std::endl;
	//	std::cout << "destructor called  and addr = " << cubes << "  x = " << posx << "  z = " << posz << std::endl;
	chunksMap.erase(GET_CHUNK_ID(posx, posz));
	if (frontNeighbour)
		frontNeighbour->backNeighbour = 0;
	if (backNeighbour)
		backNeighbour->frontNeighbour = 0;
	if (rightNeighbour)
		rightNeighbour->leftNeighbour = 0;
	if (leftNeighbour)
		leftNeighbour->rightNeighbour = 0;
	free(cubes);
	glDeleteBuffers(1, &VBO);
}


// Set the position of the chunk
void Chunk::SetPosistion(int x, int z) {
	//	std::cout << "SetPosistion called  and addr = " << cubes << "x = " << posx << "z = " << posz << std::endl;

	posx = x;
	posz = z;
	GetNeighbour();
}

// get all the neighbours of the chunk
void Chunk::GetNeighbour() {
	if ((neighbour[CHUNK_FRONT_SIDE] = GetChunk(posx - 1, posz)))

		neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_BACK_SIDE] = this;
	if ((neighbour[CHUNK_BACK_SIDE] = GetChunk(posx + 1, posz)))
		neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_FRONT_SIDE] = this;
	if ((neighbour[CHUNK_RIGHT_SIDE] = GetChunk(posx, posz + 1)))
		neighbour[CHUNK_RIGHT_SIDE]->neighbour[CHUNK_LEFT_SIDE] = this;
	if ((neighbour[CHUNK_LEFT_SIDE] = GetChunk(posx, posz - 1)))
		neighbour[CHUNK_LEFT_SIDE]->neighbour[CHUNK_RIGHT_SIDE] = this;
}

// Generate the chunk cubes data
void Chunk::Generate() {
	if (!(cubes = (unsigned char*)calloc(1, sizeof(unsigned char) * 256 * CHUNK_SIZE * CHUNK_SIZE)))
		return;
	globalChunkGeneration.generate(CHUNK_SIZE, posx, posz, cubes);
	status = CHUNK_DATA_LOADED;
}

// generate a mesh based on the chunk cube data 
void Chunk::createMeshData() {
	mesh.resize(0);

	/*for (int y = 0; y < 255; y++)
		for (int x = 0; x < CHUNK_SIZE; x++)
			for (int z = 0; z < CHUNK_SIZE; z++)
				if (cubes[GET_CUBE(x, y, z)])
					addVisibleVertices(x, y, z);*/

	GreedyMeshALL();
	verticesNumber = (unsigned int)mesh.size();
	addVisibleBorderVertices();
	status = CHUNK_LOADED;
}

// generate VAO VBO, fill the VBO and bind it to the VAO
void Chunk::Bind() {
	if (!verticesNumber) {
		threadStatus &= 0xF; // remove the CHUNK_PROCESSING byte and keep the rest
		return;
	}

	vao.Gen();

	glCreateBuffers(1, &VBO);
	glNamedBufferData(VBO, sizeof(int64_t) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(vao.ID, 0);
	glVertexArrayAttribBinding(vao.ID, 0, 0);
	glVertexArrayAttribIFormat(vao.ID, 0, 1, GL_UNSIGNED_INT, 0);

	glEnableVertexArrayAttrib(vao.ID, 1);
	glVertexArrayAttribBinding(vao.ID, 1, 0);
	glVertexArrayAttribIFormat(vao.ID, 1, 1, GL_UNSIGNED_INT, sizeof(int32_t));

	glVertexArrayVertexBuffer(vao.ID, 0, VBO, 0, sizeof(int64_t));

	threadStatus &= 0xF; // remove the CHUNK_PROCESSING byte and keep the rest
	mesh.clear();
}

void Chunk::Update() {
	if (threadStatus & CHUNK_PROCESSING || status < CHUNK_LOADED) // almost never needed be we nerver know
		return;
	mesh.clear();
	createMeshData();
	addVisibleBorderVertices();
	if (verticesNumber)
		glNamedBufferData(VBO, sizeof(int64_t) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
}

void Chunk::UpdateCube(int x, int z) { // maybe use a switch case
	Update();
	if (x == 0 && neighbour[CHUNK_FRONT_SIDE]) {
		neighbour[CHUNK_FRONT_SIDE]->Update();
		if (z == 0)
			neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_LEFT_SIDE]->Update();
		if (z == 15)
			neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->Update();
	}
	if (x == 15 && neighbour[CHUNK_BACK_SIDE]) {
		neighbour[CHUNK_BACK_SIDE]->Update();
		if (z == 0)
			neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_LEFT_SIDE]->Update();
		if (z == 15)
			neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_RIGHT_SIDE]->Update();
	}
	if (z == 0 && neighbour[CHUNK_LEFT_SIDE])
		neighbour[CHUNK_LEFT_SIDE]->Update();
	if (z == 15 && neighbour[CHUNK_RIGHT_SIDE])
		neighbour[CHUNK_RIGHT_SIDE]->Update();
}

// return a pointer to a chunk if it exist based on its coordonate and return 0 if it is not found
Chunk* GetChunk(int x, int z) {
	auto search = Chunk::chunksMap.find(GET_CHUNK_ID(x, z));

	return (search != Chunk::chunksMap.end()) ? search->second : 0;
}

void Chunk::SetPlayerProximity(const glm::vec3 &playerPos) {
	int x, z;

	x = posx - ((int)playerPos.x >> 4);
	z = posz - ((int)playerPos.z >> 4);
	this->playerProximity = (unsigned int)VEC2_LEN(x, z);
}