#include "stb_image.h"
#include "cubeMap.h"

CubeMap::CubeMap(GLuint slot) {
    Gen(slot);
}

CubeMap::CubeMap(const char **fileNames, GLuint slot) {
    Gen(slot);
    Load(fileNames);
}

void CubeMap::Gen(GLuint slot) {
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

void CubeMap::Load(const char **fileNames) {
    void* data;
    int x, y, comp;

    stbi_set_flip_vertically_on_load(false);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
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

void CubeMap::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void CubeMap::Unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::Delete() {
    glDeleteTextures(1, &ID);
    ID = 0;
}