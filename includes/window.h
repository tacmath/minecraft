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

//#define DEBUG_MODE

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/vec2.hpp>

# define DEFAULT_WINDOW_WIDTH  1700
# define DEFAULT_WINDOW_HEIGHT  1080

class Window {
private:
    GLFWmonitor *monitor;
public:
	// the window context
	GLFWwindow*     context;
	glm::vec2		size;

	Window() {
        char path[1024];

        cwd(path, 1024);
        if (!glfwInit()) {
            std::cerr << "Glfw failed to init\n";
            exit(-1);
        }
        cd(path);
        //    glfwWindowHint(GLFW_SAMPLES, 4);      // no multisampling
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
        if (!(context = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Minecraft", NULL, NULL))) {
            glfwTerminate();
            std::cerr << "Glfw failed to create a window\n";
            exit(-1);
        }
        glfwMakeContextCurrent(context);

        gladLoadGL();
        glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        glfwSetInputMode(context, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        size = glm::vec2((float)DEFAULT_WINDOW_WIDTH, (float)DEFAULT_WINDOW_HEIGHT);
        enableGlParam();

        monitor = glfwGetPrimaryMonitor();

        #ifdef DEBUG_MODE
        Debug();
        #endif // DEBUG_MODE
	}

    void FullScreen(void) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(context, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void Windowed(void) {
        glfwSetWindowMonitor(context, nullptr, 0, 0, (int)size.x, (int)size.y, 60);
    }

    void enableGlParam(void) {
    //    glDisable(GL_MULTISAMPLE);      // deactivate multisample to avoid weird texture problem with the atlas
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void Debug(void) {
        std::cout << glGetString(GL_VERSION) << std::endl;
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
            if (id != GL_INVALID_OPERATION)
                std::cout << message << " severity " << severity << " length " << length << " type " << source << " source " << type << std::endl;
            }, 0);
    }

    ~Window() {
        glfwDestroyWindow(context);
        glfwTerminate();
    }
};

#endif