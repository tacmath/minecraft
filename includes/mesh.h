#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<glad/glad.h>
#include <VAO.h>
#include<vector>

class Mesh
{
public:
	VAO VAO;

	Mesh(std::vector<unsigned int> vertices) {
	}


	// delete Mesh data
	void Delete() {
		VAO.Delete();
	}
};

#endif