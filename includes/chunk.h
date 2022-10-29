#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include<glad/glad.h>
#include <VAO.h>
#include <noise.h>
#include<vector>

#define AIR 0

#define PACK_VERTEX_DATA(x, y, z, textx, texty) (y | (x << 8) | (z << 12) | textx << 16 | texty << 17)

typedef unsigned char t_cubes[256][16][16];

#include <bitset>


void printVertexData(unsigned int vertex) {
	std::bitset<32> bit(vertex);
	std::cout << "y = " << (vertex & 0xFFu) << " x = " << (((vertex >> 8u) & 0xFu)) << " z = " << (((vertex >> 12u) & 0xFu)) << std::endl;
	std::cout << bit << '\n';
}

class Chunk
{
public:
	// ids of all the cube in the chunk
	t_cubes cubes;
	//the mesh is an array containing vertex position and other information in an unsigned int
	std::vector<unsigned int> mesh;
	// the number of all the vertices
	unsigned int verticesNumber;

	// vertex array object
	VAO VAO;

	Chunk() {
		memset(cubes, 0, sizeof(char) * 256 * 256);
		verticesNumber = 0;
	};

	void Generate(unsigned int seed) {
		Noise noise(seed);

		for (unsigned x = 0; x < 16; x++) {
			for (unsigned z = 0; z < 16; z++) {
				unsigned height = (unsigned)(1.0f + 60.0f * abs(noise.noise((20 + x) * 1.0f / 300.0f, (20 + z) * (1.0f / 300.0f))));
				for (unsigned y = 0; y < 256; y++)
					if (y < height)
						cubes[y][x][z] = 1;
			}
		}

		createMeshData(cubes);
		verticesNumber = (unsigned int)mesh.size();
		std::cout << "size = " << mesh.size() << std::endl;
		Bind();

		/*for (unsigned n = 0; n < (verticesNumber / 3); n++) {
			std::cout << std::endl << "triangle = " << std::endl;
			printVertexData(mesh[n * 3]);
			printVertexData(mesh[n * 3 + 1]);
			printVertexData(mesh[n * 3 + 2]);
		}*/
	}
	void Bind() {
		GLuint VBO;

		VAO.Gen();
		VAO.Bind();
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * verticesNumber, (void*)(&mesh[0]), GL_STATIC_DRAW);
		mesh.clear();
		glEnableVertexAttribArray(0);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, (void*)0);
		glBindVertexArray(0);
	}

	// Draw the chunk 
	void Draw(void) {
		VAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, verticesNumber);
	}

	// Deletes the EBO
	void Delete() {
		VAO.Delete();
	}

private:
	inline void addTopVertices(const int y, const int x, const int z) {
		// v.insert(v.end(), std::begin(a), std::end(a)); instead of multiples push_back
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 0));

		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 0));
	}

	inline void addBottomVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));

		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 1));
	}

	inline void addFrontVertices(const int y, const int x, const int z) {
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));


		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 1, 1));
	}

	inline void addBackVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));

		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
	}

	inline void addRightVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA(x, y, (z + 1), 0, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));

		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), (z + 1), 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, (z + 1), 1, 0));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), (z + 1), 1, 1));
	}

	inline void addLeftVertices(const int y, const int x, const int z) {

		mesh.push_back(PACK_VERTEX_DATA(x, y, z, 0, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 1, 0));

		mesh.push_back(PACK_VERTEX_DATA((x + 1), y, z, 1, 0));
		mesh.push_back(PACK_VERTEX_DATA(x, (y + 1), z, 0, 1));
		mesh.push_back(PACK_VERTEX_DATA((x + 1), (y + 1), z, 1, 1));
	}

	inline void addVisibleVertices(t_cubes cubes, int x, int y, int z) {
		if (y == 255 || cubes[y + 1][x][z] == AIR)
			addTopVertices(y, x, z);
		if (y == 0 || cubes[y - 1][x][z] == AIR)
			addBottomVertices(y, x, z);
		if (x == 0 || cubes[y][x - 1][z] == AIR)
			addFrontVertices(y, x, z);
		if (x == 14 || cubes[y][x + 1][z] == AIR)
			addBackVertices(y, x, z);
		if (z == 0 || cubes[y][x][z - 1] == AIR)
			addLeftVertices(y, x, z);
		if (z == 14 || cubes[y][x][z + 1] == AIR)
			addRightVertices(y, x, z);
	}

	void createMeshData(t_cubes cubes) {

		for (int y = 0; y < 255; y++)
			for (int x = 0; x < 15; x++)
				for (int z = 0; z < 15; z++)
					if (cubes[y][x][z])
						addVisibleVertices(cubes, x, y, z);
	}

};

#endif