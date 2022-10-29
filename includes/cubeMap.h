#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include <glad/glad.h>
#include <iostream>
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

    // Generate a cubemap texture ID and bind it to a texture unit 
    CubeMap(GLuint slot);
    // Generate , bind to a texture unit and load a cubemap texture 
    CubeMap(const char** fileNames, GLuint slot);

 //   void Load(const char** fileNames);

    // Bind the cubemap
    void Bind();
    // Unbind the cubemap
    void Unbind();
    // Delete the cubemap
    void Delete();
};

CubeMap::CubeMap(GLuint slot) {
    unit = slot;
    glActiveTexture(GL_TEXTURE0 + unit);
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

CubeMap::CubeMap(const char** fileNames, GLuint slot) {
    void* data;
    int x, y, comp;

    unit = slot;
    stbi_set_flip_vertically_on_load(false);
    glActiveTexture(GL_TEXTURE0 + unit);
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (int n = 0; n < 6; n++) {
        data = stbi_load(fileNames[n], &x, &y, &comp, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + n, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
            std::cerr << "Failed to load" << fileNames[n] << std::endl;
    }
}

/*void CubeMap::Load(const char** fileNames) {


}*/

void CubeMap::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void CubeMap::Unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::Delete() {
    glDeleteTextures(1, &ID);
}

#endif