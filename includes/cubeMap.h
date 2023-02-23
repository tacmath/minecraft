#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include <glad/glad.h>
#include <iostream>
#include <array>
#include <string>
/*
struct texture_data {
    void* data;
    int x;
    int y;
    int comp;
};*/

class CubeMap {
/*
private:
    texture_data textures[6];*/

public:
    // Reference ID of the texture
    GLuint ID;
    // Unit where the texture is assigned 
    GLuint unit;

    // Default constructor
    CubeMap() {
        ID = 0;
        unit = 0;
    };
    // Constructor that generate a cubemap texture ID and bind it to a texture unit 
    CubeMap(GLuint slot);
    // Constructor that generate , bind to a texture unit and load a cubemap texture 
    CubeMap(const std::array<std::string, 6> fileNames, GLuint slot);

    // Default destructor
    ~CubeMap() { glDeleteTextures(1, &ID); };

    // Generate a cubemap texture ID and bind it to a texture unit
    CubeMap& Gen(GLuint slot);
    // Load the cubemap and bind it to a texture unit
    CubeMap& Load(const std::array<std::string, 6> fileNames);
    // Bind the cubemap
    void Bind();
    // Unbind the cubemap
    void Unbind();
    // Delete the cubemap
    void Delete();
};

#endif