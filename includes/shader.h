#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<fstream>
#include<iostream>
#include<glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // Reference ID of the Shader Program
    GLuint ID;
    // default constuctor
    Shader() {
        ID = 0;
    }
    // Constructor that build the Shader Program from 2 different shaders
    Shader(const char* vertexFile, const char* fragmentFile);

    void Load(const char* vertexFile, const char* fragmentFile) {
        Shader shader(vertexFile, fragmentFile);

        this->ID = shader.ID;
    }

    void Activate() {
        glUseProgram(ID);
    }

    void Delete() {
        glDeleteProgram(ID);
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
    void setVec2(const char* name, float x, float y) {
        glUniform2f(glGetUniformLocation(ID, name), x, y);
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

    static char* getShaderSource(const char* fileName) {
        char* source;
        std::ifstream   file;
        size_t  size;

        file.open(fileName);
        if (!file.is_open()) {
            printf("Failed to open %s\n", fileName);
            return (0);
        }
        file.seekg(0, std::ios::end);
        size = file.tellg();
        file.seekg(0);
        source = new char[size + 1];
        source[size] = 0;
        file.read(source, size);
        file.close();
        return (source);
    }
};

Shader::Shader(const char *vertexShaderFile, const char *fragmentShaderFile) {
    char* vertexShaderSource = getShaderSource(vertexShaderFile);
    char* fragmentShaderSource = getShaderSource(fragmentShaderFile);

    ID = 0;
    if (!vertexShaderSource || !fragmentShaderSource)
        return ;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

	GLuint programShader = glCreateProgram();
	glAttachShader(programShader, vertexShader);
	glAttachShader(programShader, fragmentShader);
	glLinkProgram(programShader);
    compileErrors(programShader, "PROGRAM");

    delete vertexShaderSource;
    delete fragmentShaderSource;
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    ID = programShader;
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
    // Stores status of compilation
    GLint hasCompiled;
    // Character array to store error message in
    char infoLog[1024];
    if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
        }
        return ;
    }
    glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
    if (hasCompiled == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
    }
}

#endif

