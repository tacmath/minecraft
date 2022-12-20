#ifndef WINDOW_H
#define WINDOW_H

#ifdef _WIN32
#include <direct.h>
// MSDN recommends against using getcwd & chdir names
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

# define DEFAULT_WINDOW_WIDTH  1700
# define DEFAULT_WINDOW_HEIGHT  1080

class Window {
public:
	// the window context
	GLFWwindow*     context;
	glm::vec2		size;

	Window() {
        char path[1024];

        cwd(path, 1024);
        if (!glfwInit()) {
            printf("Glfw failed to init\n");
            exit(-1);
        }
        cd(path);
        //    glfwWindowHint(GLFW_SAMPLES, 4);      // no multisampling
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
        if (!(context = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Minecraft", NULL, NULL))) {
            glfwTerminate();
            printf("Glfw failed to create a window\n");
            exit(-1);
        }
        glfwMakeContextCurrent(context);

        gladLoadGL();
        glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        glfwSetInputMode(context, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        size = glm::vec2((float)DEFAULT_WINDOW_WIDTH, (float)DEFAULT_WINDOW_HEIGHT);
	}

    ~Window() {
        glfwDestroyWindow(context);
        glfwTerminate();
    }
};

#endif