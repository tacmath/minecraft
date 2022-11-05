#include "chunk.h"

// Default constructor
Chunk::Chunk() {
	//	std::cout << "constructor called" << std::endl;
	verticesNumber = 0;
	posx = 0;
	posz = 0;
	cubes = 0;
	VBO = 0;
	status = CHUNK_UNLOADED;
	threadStatus = CHUNK_NOT_PROCESSING;
	neighbourLoaded = CHUNK_NONE;
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

// return a pointer to a chunk if it exist based on its coordonate and return 0 if it is not found
Chunk* Chunk::GetChunk(int x, int z) {
	auto search = chunksMap.find(GET_CHUNK_ID(x, z));
	if (search != chunksMap.end())
		return (search->second);
	else
		return (0);
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
	if (neighbourLoaded != CHUNK_NONE)
		addVisibleBorderVertices(neighbourLoaded);
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
	if (neighbourLoaded == CHUNK_ALL_LOADED)
		mesh.clear();
}

// Draw the chunk 
void Chunk::Draw(Shader& shader) {
	VAO.Bind();
	shader.setVec2("chunkPos", (float)(posx << 4), (float)(posz << 4));
	glDrawArrays(GL_TRIANGLES, 0, verticesNumber);
}

//add the mesh between a chunk and its neighbours
void Chunk::addNeighbours() {
	char sides = 0;

	for (int n = 0; n < 4; n++)
		// if the neighbour exist and has loaded cubes and was not processed earlier
		if (neighbour[n] && neighbour[n]->status >= CHUNK_DATA_LOADED && !((neighbourLoaded >> n) & 1))
			sides |= 1 << n;
	// if no new side has been found quit
	if (sides == CHUNK_NONE)
		return;
	addVisibleBorderVertices(sides);
	if (verticesNumber) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
	}
	neighbourLoaded |= sides;
	if (neighbourLoaded == CHUNK_ALL_LOADED)
		mesh.clear();
}