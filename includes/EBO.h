#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>
#include<vector>

template <typename T = GLuint>
class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(std::vector<T>& indices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(T), indices.data(), GL_STATIC_DRAW);
	}

	// Binds the EBO
	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	}

	// Unbinds the EBO
	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Deletes the EBO
	void Delete()
	{
		glDeleteBuffers(1, &ID);
	}
};

#endif