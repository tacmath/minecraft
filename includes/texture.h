#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <iostream>

#define TEX_ATLAS_WIDTH 3
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

Texture::~Texture() {
    glDeleteTextures(1, &ID);
}

Texture::Texture(const std::string fileName, GLuint slot) {
    this->Load(fileName, slot);
}

void Texture::Load(const std::string fileName, GLuint slot) {
    void* data;
    int x, y, comp;

    unit = slot;
    stbi_set_flip_vertically_on_load(true);
    if (!(data = stbi_load(fileName.c_str(), &x, &y, &comp, 0))) {
        std::cerr << "Failed to load" << fileName << std::endl;
        ID = 0;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void Texture::LoadAtlas(const std::vector<std::string> &fileNames, GLuint slot) {
    void* data;
    int x, y, comp;

    unit = slot;
    glActiveTexture(GL_TEXTURE0 + unit);
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // limit the mimap level for each block in the atlas to be at 1 pixel minimum
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (int)sqrt(TEXTURE_SIZE));
    
    //create the atlas texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE * TEX_ATLAS_WIDTH, TEXTURE_SIZE * TEX_ATLAS_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); //change for alpha
    
    stbi_set_flip_vertically_on_load(true);
    for (unsigned n = 0; n < fileNames.size(); n++) {
        if (!(data = stbi_load(fileNames[n].c_str(), &x, &y, &comp, 0)))
            std::cerr << "Failed to load" << fileNames[n] << std::endl;
        if (n < TEX_ATLAS_WIDTH * TEX_ATLAS_HEIGHT)
            glTexSubImage2D(GL_TEXTURE_2D, 0, n * TEXTURE_SIZE, 0, x, y, (comp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    //create the atlas mimap
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() {
    glDeleteTextures(1, &ID);
    ID = 0;
}

void Texture::fillMipmaps(void* data, int width, int height, int offsetx, int offsety, int levels, int comp) {
    unsigned char* imageData = (unsigned char*)data;

    for (int mipLevel = 0; mipLevel < levels; mipLevel++) {
        int mipsize = width >> 1;

        if (!(width >> 1)  || !(height >> 1))
            return;
        std::vector<unsigned char> mipmap;
        mipmap.resize(mipsize * (height >> 1) * 3);
        memset(&mipmap[0], 0, mipmap.size() * sizeof(char));
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                for (int n = 0; n < 3; n++)
                    mipmap[((y >> 1) * mipsize + (x >> 1)) * 3 + n] += imageData[(y * width + x) * comp + n] / 4;

        width >>= 1;
        height >>= 1;
        offsetx >>= 1;
        offsety >>= 1;
        comp = 3;
        glTexSubImage2D(GL_TEXTURE_2D, mipLevel + 1, offsetx, offsety, width, height, GL_RGB, GL_UNSIGNED_BYTE, &mipmap[0]);
        std::copy(mipmap.begin(), mipmap.end(), imageData);
    }
}

#endif