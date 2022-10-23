#include "scop.h"

void getNormalKeyEvent(t_scop *scop) {
    static char keyStatusN = 1;
    static char normalMap = 1;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_N);
    if (status == GLFW_PRESS && keyStatusN) {
        normalMap = !normalMap;
        glUniform1i(glGetUniformLocation(scop->object.programShader, "activateNormalMap"), normalMap);
        keyStatusN = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatusN = 1;
}

void getMouseEvent(t_scop *scop) {
    int state;
    double posx, posy;

    state = glfwGetMouseButton(scop->window, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(scop->window, &posx, &posy);
    if (state == GLFW_PRESS)
    {
        t_mat4 matrix = IDENTITY_MAT4;
        t_mat4 tmp;
        float  rotationY;
        
        scop->object.rotation.y += (GLfloat)posx - scop->mouse.x;
        if (fabs(scop->object.rotation.x + (GLfloat)posy - scop->mouse.y) < 90)
            scop->object.rotation.x += (GLfloat)posy - scop->mouse.y;
        rotationY = scop->object.rotation.y * (GLfloat)(PI / 180.0f);
        rotate(matrix, scop->object.rotation.y, (t_vertex){0.0f, 1.0f, 0.0f}, &tmp);
        rotate(tmp, scop->object.rotation.x, (t_vertex){cosf(rotationY), 0.0f, sinf(rotationY)}, &scop->rotation);
    }
    scop->mouse.x = (GLfloat)posx;
    scop->mouse.y = (GLfloat)posy;
}

void getTransitionKeyEvent(t_scop *scop) {
    static char transition = -1;
    static char keyStatusT = 1;
    int status;

    status = glfwGetKey(scop->window, GLFW_KEY_T);
    if (status == GLFW_PRESS && keyStatusT) {
        transition = -transition;
        keyStatusT = 0;
    }
    else if (status == GLFW_RELEASE)
        keyStatusT = 1;
    if (scop->transition >= 0 && transition == -1)
        scop->transition -= 0.02f;
    else if (scop->transition <= 1 && transition == 1)
        scop->transition += 0.02f;
}

void getEvents(t_scop *scop) {
    glfwPollEvents();
    getMouseEvent(scop);
    getNormalKeyEvent(scop);
    getTransitionKeyEvent(scop);
}