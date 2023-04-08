#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H

#define DEBUG_MODE 1

#if DEBUG_MODE == 1
#define ON_DEBUG(x) x
#define RENDERING_INFO 0
#else
#define ON_DEBUG(x)
#endif // DEBUG_MODE == 1


#if RENDERING_INFO == 1

struct global_render_info {
    int nb_drawcall;
    int polygon_count;

    global_render_info() {
        nb_drawcall = 0;
        polygon_count = 0;
    }
};

extern global_render_info mc_info;

#define McDrawArrays(type, offset, verticesNumber) glDrawArrays(type, offset, verticesNumber); \
        mc_info.nb_drawcall++; \
        mc_info.polygon_count += (verticesNumber - offset) / 3
#else
#define McDrawArrays(type, offset, verticesNumber) glDrawArrays(type, offset, verticesNumber)
#endif

/*
#include <GLFW/glfw3.h>
#include <stdio.h>

#define GET_EXEC_TIME(code) \
    do { \
    float startTime = (float)glfwGetTime(); \
    code; \
    float stopTime = (float)glfwGetTime(); \
    printf("Elapsed: %f ms\n", (float)(stopTime - startTime) * 1000); \
    } while(0)

#define GET_RENDER_TIME(code) \
    do { \
    glFinish(); \
    float startTime = (float)glfwGetTime(); \
    code; \
    float stopTime = (float)glfwGetTime(); \
    printf("Compute: %f ms\n", (float)(stopTime - startTime) * 1000); \
    startTime = stopTime; \
    glFinish(); \
    stopTime = (float)glfwGetTime(); \
    printf("Render left: %f ms\n", (float)(stopTime - startTime) * 1000); \
    } while(0)
*/
#endif