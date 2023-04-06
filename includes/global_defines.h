#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H

#define DEBUG_MODE 1
#define RENDERING_INFO 1

#if DEBUG_MODE == 1
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif // DEBUG_MODE == 1

/*
#if RENDERING_INFO == 1
extern int nb_drawcall;
extern int polygon_count;
#define MCDrawArrays(type, offset, verticesNumber) glDrawArrays(type, offset, verticesNumber); \
        nb_drawcall++; \
        polygon_count += verticesNumber / 3
#else
#define MCDrawArrays(type, offset, verticesNumber) glDrawArrays(type, offset, verticesNumber)
#endif


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
    glFinish(); \
    printf("Elapsed: %f ms\n", (float)(stopTime - startTime) * 1000); \
    } while(0)
*/
#endif