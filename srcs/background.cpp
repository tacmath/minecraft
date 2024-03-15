#include "background.h"

Background::Background(void) {
    float   vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };

    vbo.Gen(vertices, sizeof(vertices));
    vao.Gen();
    vao.LinkAttrib(vbo, 0, 2, GL_FLOAT, sizeof(float), 0);
    shader.Load("shaders/skyVS.glsl", "shaders/skyFS.glsl");
}

// destructor
Background::~Background(void) {
    shader.Delete();
    vao.Delete();
    vbo.Delete();
}

// draw the skybox
void Background::Draw(void) {
    glDepthMask(false);
    glDepthFunc(GL_LEQUAL);
    shader.Activate();
    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    stars.Draw();
    sun.Draw();
    glDepthFunc(GL_LESS);
    glDepthMask(true);
}

void Background::LoadSunPos(const glm::vec3& sunPos) {
    shader.setVec3("sunPos", sunPos);
    stars.LoadSunPos(sunPos);
}

//load the view matrix in all the shaders
void Background::LoadViewMatrix(const Camera& camera) {
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));

    shader.setMat4("view", view);
    stars.SetVP(camera);
    sun.SetView(view);

    glm::mat4 IVP = glm::inverse(camera.GetProjection() * view);
    shader.setMat4("IVP", IVP);
}

void Background::initUniforms(const Camera& camera) {
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));

    shader.setInt("skybox", 0);
    shader.setMat4("projection", camera.projection);
    shader.setMat4("view", view);
    stars.SetVP(camera);
    sun.SetView(view);
    sun.SetProjection(camera.projection);

    glm::mat4 IVP = glm::inverse(camera.GetProjection() * view);
    shader.setMat4("IVP", IVP);
}