#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>


#define TEX_ATLAS_WIDTH 4
#define TEX_ATLAS_HEIGHT 1

#define TEXTURE_SIZE 16

class Texture {
public:
    // Reference ID of the texture
    GLuint ID;
    // Unit where the texture is assigned 
    GLuint unit;
    // Default constructor
    Texture() {
        ID = 0;
        unit = 0;
    }
    // Default destructor
    ~Texture();
    // Constructor that create and load a new texure and bind it to a texture unit
    Texture(const std::string fileName, GLuint slot);

    // load a new texure and bind it to a texture unit
    void Load(const std::string fileName, GLuint slot);

    // load all texture in fileNames and create a texture atlas 
    void LoadAtlas(const std::vector<std::string> &fileNames, GLuint slot);
    // Bind a texture to a texture unit
    void Bind();
    // Unbind a texture
    void Unbind();
    // Delete a texture
    void Delete();

private:
    // Fill mipmaps at a certain position in the texture 
    void fillMipmaps(void *data, int width, int height, int offsetx, int offsety, int levels, int comp);
};

#endif