#ifndef BACKGROUND_H
# define BACKGROUND_H

#include "shader.h"
#include "VAO.h"
#include "EBO.h"
#include "texture.h"
#include "cubeMap.h"
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Background {
private:
    // skybox vexter array object
	VAO     VAO;
    // skybox element buffer object
	EBO     skyboxEBO;
    // skybox shader
    Shader  shader;
    // skybox cubemap
    CubeMap cubemap;
public:

    // constuctor
	Background(void);

    // destructor
    ~Background(void);

    // draw the skybox
    void Draw(void);

    //load the view matrix in all the shaders
    void LoadViewMatrix(Camera& camera);

    void initUniforms(Camera& camera);

private:
    void initSkybox(void);
};

#endif