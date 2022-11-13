#include "chunk.h"
#include "chunk_generation.h"

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
	neighbour.resize(4);
	memset(&neighbour[0], 0, sizeof(Chunk*) * 4);
};

// Destructor
Chunk::~Chunk() {
	//	std::cout << "chunk has been destroyed" << std::endl;
	//	std::cout << "destructor called  and addr = " << cubes << "  x = " << posx << "  z = " << posz << std::endl;
	chunksMap.erase(GET_CHUNK_ID(posx, posz));
	if (neighbour[CHUNK_FRONT_SIDE])
		neighbour[CHUNK_FRONT_SIDE]->neighbour[CHUNK_BACK_SIDE] = 0;
	if (neighbour[CHUNK_BACK_SIDE])
		neighbour[CHUNK_BACK_SIDE]->neighbour[CHUNK_FRONT_SIDE] = 0;
	if (neighbour[CHUNK_RIGHT_SIDE])
		neighbour[CHUNK_RIGHT_SIDE]->neighbour[CHUNK_LEFT_SIDE] = 0;
	if (neighbour[CHUNK_LEFT_SIDE])
		neighbour[CHUNK_LEFT_SIDE]->neighbour[CHUNK_RIGHT_SIDE] = 0;
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
	for (int y = 0; y < 255; y++)
		for (int x = 0; x < CHUNK_SIZE; x++)
			for (int z = 0; z < CHUNK_SIZE; z++)
				if (cubes[GET_CUBE(x, y, z)])
					addVisibleVertices(x, y, z);

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
	VAO.Gen();
	VAO.Bind();
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, (void*)0);
	glBindVertexArray(0);
	threadStatus &= 0xF; // remove the CHUNK_PROCESSING byte and keep the rest
	isVisible = true;
	mesh.clear();
}

// Draw the chunk 
void Chunk::Draw(Shader& shader) {
	if (!isVisible)
		return;
	VAO.Bind();
	shader.setVec2("chunkPos", (float)(posx << 4), (float)(posz << 4));
	glDrawArrays(GL_TRIANGLES, 0, verticesNumber);
}

void Chunk::Update() {
	if (threadStatus & CHUNK_PROCESSING || status < CHUNK_LOADED) // almost never needed be we nerver know
		return;
	mesh.clear();
	createMeshData();
	addVisibleBorderVertices();
	if (verticesNumber) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
	}
}

void Chunk::UpdateCube(int x, int z) { // need tu uptade de corner if a block is in the corner for the AO
	Update();
	if (x == 0 && neighbour[CHUNK_FRONT_SIDE])
		neighbour[CHUNK_FRONT_SIDE]->Update();
	if (x == 15 && neighbour[CHUNK_BACK_SIDE])
		neighbour[CHUNK_BACK_SIDE]->Update();
	if (z == 0 && neighbour[CHUNK_LEFT_SIDE])
		neighbour[CHUNK_LEFT_SIDE]->Update();
	if (z == 15 && neighbour[CHUNK_RIGHT_SIDE])
		neighbour[CHUNK_RIGHT_SIDE]->Update();
}

// return a pointer to a chunk if it exist based on its coordonate and return 0 if it is not found
Chunk* GetChunk(int x, int z) {
	auto search = chunksMap.find(GET_CHUNK_ID(x, z));
	if (search != chunksMap.end())
		return (search->second);
	else
		return (0);
}