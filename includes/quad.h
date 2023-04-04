#ifndef QUAD_H
#define QUAD_H

#include <glm/vec2.hpp>
#include <glad/glad.h>

class Quad {
	GLuint VAO;
	GLuint VBO;

public:

	Quad() {
		VAO = 0;
		VBO = 0;
	};

	~Quad() {
		Delete();
	};

	void Init(GLuint usage = GL_STATIC_DRAW) {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
		};
		// setup plane VAO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, usage);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}

	void Init(glm::vec2 start, glm::vec2 len, GLuint usage = GL_STATIC_DRAW)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			start.x,           start.y + len.y,   0.0f, 0.0f, 1.0f,
			start.x,           start.y,           0.0f, 0.0f, 0.0f,
			start.x + len.x,   start.y + len.y,   0.0f, 1.0f, 1.0f,
			start.x + len.x,   start.y,           0.0f, 1.0f, 0.0f
		};
		// setup plane VAO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, usage);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}

	void Render() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void Delete() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
		VBO = 0;
	};
};

#endif