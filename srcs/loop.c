#include "scop.h"

extern float cameraPosZ;

void initUniforms(t_scop *scop) {
    t_vertex objectSize =   {scop->object.mesh.max.x - scop->object.mesh.min.x,
                            scop->object.mesh.max.y - scop->object.mesh.min.y,
                            scop->object.mesh.max.z - scop->object.mesh.min.z};


    glUseProgram(scop->background.programShader);
    glUniform1i(glGetUniformLocation(scop->background.programShader, "backgroundTex"), 0);

    glUseProgram(scop->object.programShader);
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "Osize"), 1, (void*)(&objectSize));
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "Omin"), 1, (void*)(&scop->object.mesh.min));
    glUniform3fv(glGetUniformLocation(scop->object.programShader, "lightPos"), 1, (void*)(&scop->lightPos));
    glUniform1i(glGetUniformLocation(scop->object.programShader, "activateNormalMap"), 1);
    glUniform1i(glGetUniformLocation(scop->object.programShader, "Texture"), 0); 
    glUniform1i(glGetUniformLocation(scop->object.programShader, "NormalMap"), 1);
}

void setModelMatrix(t_scop *scop, GLuint matrixLoc) {
    t_mat4 matrix = IDENTITY_MAT4;

    mat4Traslate(&matrix, (t_vertex){
                /*- ((scop->object.mesh.max.x - scop->object.mesh.min.x) / 2) - scop->object.mesh.min.x*/0.0f,
                - ((scop->object.mesh.max.y - scop->object.mesh.min.y) / 2) - scop->object.mesh.min.y,
                - ((scop->object.mesh.max.z - scop->object.mesh.min.z) / 2) - scop->object.mesh.min.z});
    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)matrix);
}

void setVPMatrix(t_scop *scop, GLuint matrixLoc) {
    t_mat4 matrix = IDENTITY_MAT4;
    t_mat4 view = IDENTITY_MAT4;

    mat4Traslate(&view, (t_vertex){0.0f, 0.0f, cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2});
    mat4Mult(view, scop->rotation, &matrix);
    mat4Mult(scop->projection, matrix, &view);
    glUniformMatrix4fv(matrixLoc, 1, GL_TRUE, (GLfloat*)view); 
}

void mainLoop(t_scop *scop) {
    GLuint matrixLoc =  glGetUniformLocation(scop->object.programShader, "matrix");
    GLuint cameraPosLoc =  glGetUniformLocation(scop->object.programShader, "cameraPos");
    GLuint modelMatrixLoc =  glGetUniformLocation(scop->object.programShader, "model");
    GLuint transitionLoc = glGetUniformLocation(scop->object.programShader, "transition");
    t_mat4 matrix;
   
    initUniforms(scop);
    while ( glfwGetKey(scop->window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(scop->window) == 0 ) {
        
        glUniform1f(transitionLoc, scop->transition);
        t_vertex carmeraPos = (t_vertex){0.0f, 0.0f, -(cameraPosZ - (scop->object.mesh.max.y - scop->object.mesh.min.y) * 2)};
        memcpy(matrix, scop->rotation, sizeof(t_mat4));
        mat4Transpose(&matrix);
        carmeraPos = mat4Vec3Mult(matrix, carmeraPos);
    //    printf("v = %f, %f, %f\n", carmeraPos.x, carmeraPos.y, carmeraPos.z);
        glUniform3fv(cameraPosLoc, 1, (void*)(&carmeraPos));
    //    glUniform3fv(glGetUniformLocation(scop->object.programShader, "lightPos"), 1, (void*)(&carmeraPos));
        setVPMatrix(scop, matrixLoc);

        glClear(GL_DEPTH_BUFFER_BIT);
        drawBackground(scop);
        drawObject(scop, modelMatrixLoc);
		glfwSwapBuffers(scop->window);
        getEvents(scop);
        limitFPS();
    }
}