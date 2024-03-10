#ifndef BACKGROUND_H
# define BACKGROUND_H

#include "stars.h"
#include "shader.h"
#include "VAO.h"
#include "EBO.h"
#include "texture.h"
#include "camera.h"
#include "sun.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Background {
private:
    // sky vexter buffer object
    VBO     vbo;
    // sky vexter array object
	VAO     vao;
    // sky shader
    Shader  shader;
    // sky stars
    Stars stars;
public:
    // the sun
    Sun     sun;

    // constuctor
	Background(void);

    // destructor
    ~Background(void);

    // draw the skybox
    void Draw(void);

    void LoadSunPos(const glm::vec3& sunPos);

    //load the view matrix in all the shaders
    void LoadViewMatrix(const Camera& camera);

    void initUniforms(const Camera& camera);

};

#endif