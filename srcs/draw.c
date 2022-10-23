#include "scop.h"

void drawBackground(t_scop *scop) {
        glDisable(GL_DEPTH_TEST);
        glUseProgram(scop->background.programShader);
        glBindVertexArray(scop->background.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scop->background.textureID);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);
}

void drawObject(t_scop *scop, GLuint modelMatrixLoc) {
    glUseProgram(scop->object.programShader);
    setModelMatrix(scop, modelMatrixLoc);
    for (GLuint n = 0; n < scop->object.segmentNb; n++) {
        glBindVertexArray(scop->object.segments[n].VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scop->object.segments[n].textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, scop->object.segments[n].normalTextureID);
        if (scop->object.mesh.indices.size)
            glDrawElements(GL_TRIANGLES, scop->object.mesh.indices.size , GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, scop->object.mesh.vertices.size);
    }
}