#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<fstream>
#include<iostream>
#include<glm/gtc/type_ptr.hpp>

class Shader {
public:
    // Reference ID of the Shader Program
    GLuint ID;
    // default constuctor
    Shader() { ID = 0; }
    // default destructor
    ~Shader() { glDeleteProgram(ID); }
    // Constructor that build the Shader Program from 2 different shaders
    Shader(const char* vertexFile, const char* fragmentFile) { Load(vertexFile, fragmentFile); };

    void Load(const char* vertexFile, const char* fragmentFile, const char* geometryShaderFile = 0);

    void Activate() {
        glUseProgram(ID);
    }

    void Delete() {
        glDeleteProgram(ID);
        ID = 0;
    }
    
    void setMat4(const char* name, const glm::mat4& matrix) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &matrix[0][0]);
    }
    void setVec3(const char* name, const glm::vec3& vector) {
        glUniform3fv(glGetUniformLocation(ID, name), 1, &vector[0]);
    }
    void setVec3(const char* name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }
    inline void setVec2(const char* name, float x, float y) {
        glUniform2f(glGetUniformLocation(ID, name), x, y);
    }
    inline void setVec2(const char* name, int x, int y) {
        glUniform2i(glGetUniformLocation(ID, name), x, y); //maybe change it to calculate glGetUniformLocation only once
    }
    void setInt(const char* name, int value) {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }
    void setFloat(const char* name, float value) {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

private:
    // Checks if the different Shaders have compiled properly
    void compileErrors(unsigned int shader, const char* type);

    char* getShaderSource(const char* fileName);
};

#endif

