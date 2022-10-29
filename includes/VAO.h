#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include <VBO.h>

class VAO
{
public:
    // Reference ID of the VAO
    GLuint ID;

    // Default constructor
    VAO() {
        ID = 0;
    };
    // Generates a VAO ID
    void Gen();
    // Links Attribute such as a position or color to the VAO
    void LinkAttrib(const void *data, GLuint vertexNumbers, GLuint layout, GLuint numComponents, GLuint type, GLuint typeSize, void* offset);
    // Links VBO Attribute such as a position or color to the VAO
    void LinkAttrib(VBO &VBO, GLuint layout, GLuint numComponents, GLuint type, GLuint typeSize, void* offset);
    // Binds the VAO
    void Bind();
    // Unbinds the VAO
    void Unbind();
    // Deletes the VAO
    void Delete();
};

void VAO::LinkAttrib(const void* data, GLuint vertexNumbers, GLuint layout, GLuint numComponents, GLuint type, GLuint typeSize, void* offset) {
    GLuint VBO;

    glBindVertexArray(ID);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numComponents * typeSize * vertexNumbers, data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, numComponents * typeSize, offset);
    glBindVertexArray(0);
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLuint type, GLuint typeSize, void* offset) {

    glBindVertexArray(ID);
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, numComponents * typeSize, offset);
    glBindVertexArray(0);
}


void VAO::Gen() {
    glGenVertexArrays(1, &ID);
}

void VAO::Bind() {
    glBindVertexArray(ID);
}

void VAO::Unbind() {
    glBindVertexArray(0);
}

void VAO::Delete() {
    glDeleteVertexArrays(1, &ID);
}

#endif