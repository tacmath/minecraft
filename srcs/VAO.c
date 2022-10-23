#include "scop.h"

GLuint initVertexArray(t_array vertices) {
    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size * sizeof(t_vertex), vertices.data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return (VAO);
}

void initElementArray(GLuint VAO, t_array indices) {
    GLuint EBO;

    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * sizeof(GLuint), indices.data, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void addArrayBuffer(GLuint VAO, t_array array, int dataSize, int arrayIndex) {
    GLuint VBO;

    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, array.size * dataSize, array.data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(arrayIndex);
    glVertexAttribPointer(arrayIndex, 3, GL_FLOAT, GL_FALSE, dataSize, (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}