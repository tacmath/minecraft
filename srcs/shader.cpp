#include "shader.h"


char* Shader::getShaderSource(const char* fileName) {
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
    source = (char*)malloc(size + 1);
    source[size] = 0;
    file.read(source, size);
    for (size_t i = size - 1; i > 0; i--) {
        if (source[i] == '}') {
            source[i + 1] = 0;
            break;
        }
    }
    file.close();
    return (source);
}

void Shader::Load(const char *vertexShaderFile, const char *fragmentShaderFile) {
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

    free(vertexShaderSource);
    free(fragmentShaderSource);
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
    if (!strcmp(type, "PROGRAM"))
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