#include "shader.h"
#include "global_defines.h"

#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

char* Shader::getShaderSource(const char* fileName) {
    char* source;
    std::ifstream   file;
    size_t  size;

    if (!fileName)
        return (0);
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

void Shader::Load(const char *vertexShaderFile, const char *fragmentShaderFile, const char *geometryShaderFile) {
    std::vector<std::string> options;
    Load(options, vertexShaderFile, fragmentShaderFile, geometryShaderFile);
}

 
static char* addOptionToShaderSource(const std::vector<std::string> &options, char *source) {
    std::string fileSource;
    std::string shaderOptions = "";
    
    if (options.empty() || !source)
        return source;
    fileSource = source;
    for (std::string option : options)
        shaderOptions += "#define " + option + '\n';
    fileSource.insert(fileSource.find_first_of('\n') + 1, shaderOptions);
    free(source);
    return STRDUP(fileSource.c_str());
}

void Shader::Load(const std::vector<std::string> &options, const char *vertexShaderFile, const char *fragmentShaderFile, const char *geometryShaderFile) {
    char* vertexShaderSource = addOptionToShaderSource(options, getShaderSource(vertexShaderFile));
    char* fragmentShaderSource = addOptionToShaderSource(options, getShaderSource(fragmentShaderFile));
    char* geometryShaderSource = addOptionToShaderSource(options, getShaderSource(geometryShaderFile));

    ID = 0;
    if (!vertexShaderSource || !fragmentShaderSource)
        return ;

    LoadSources(vertexShaderSource, fragmentShaderSource, geometryShaderSource);
    free(vertexShaderSource);
    free(fragmentShaderSource);
    free(geometryShaderSource);
    ON_DEBUG(std::cout << "shader " << ID << " created with sources VS " << vertexShaderFile << " FS " << fragmentShaderFile << std::endl;)
}

void Shader::LoadSources(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    GLuint geometryShader = 0;
    if (geometryShaderSource) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
        glCompileShader(geometryShader);
        compileErrors(geometryShader, "GEOMETRY");
    }

    GLuint programShader = glCreateProgram();
    glAttachShader(programShader, vertexShader);
    glAttachShader(programShader, fragmentShader);
    if (geometryShader)
        glAttachShader(programShader, geometryShader);
    glLinkProgram(programShader);
    compileErrors(programShader, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
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