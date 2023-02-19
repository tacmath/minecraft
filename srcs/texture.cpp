#include "texture.h"
#include "stb_image.h"
#include <cstring>

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, (comp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
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
    
    //try to add anisotropic filtering and use a ifdef

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

void Texture::LoadArray(const std::vector<std::string>& fileNames, GLuint slot) {
    void* data;
    int x, y, comp;

    unit = slot;
    type = GL_TEXTURE_2D_ARRAY;
    glActiveTexture(GL_TEXTURE0 + unit);
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //anisotropic filtering
    GLfloat value, max_anisotropy = 4.0f; /* don't exceed this value...*/
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, & value);
    value = (value > max_anisotropy) ? max_anisotropy : value;
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
    
    stbi_set_flip_vertically_on_load(true);
    for (unsigned n = 0; n < fileNames.size(); n++) {
        if (!(data = stbi_load(fileNames[n].c_str(), &x, &y, &comp, 0)))
            std::cerr << "Failed to load" << fileNames[n] << std::endl;
        if (n == 0)
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, x, y, (GLuint)fileNames.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, n, x, y, 1,  (comp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    
    //create the atlas mimap
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(type, ID);
}

void Texture::Unbind() {
    glBindTexture(type, 0);
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
