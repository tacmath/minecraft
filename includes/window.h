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
#include <stb_image.h>

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
        SetWiewPort();
        glfwSetInputMode(context, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(context, DEFAULT_WINDOW_WIDTH / 2.0, DEFAULT_WINDOW_HEIGHT / 2.0);
        size = glm::vec2((float)DEFAULT_WINDOW_WIDTH, (float)DEFAULT_WINDOW_HEIGHT);
        enableGlParam();

        // load game icon in the window
        GLFWimage icons;
        icons.pixels = stbi_load("texture/icon.png", &icons.width, &icons.height, 0, 4);
        if (icons.pixels)
            glfwSetWindowIcon(context, 1, &icons);
        stbi_image_free(icons.pixels);


        monitor = glfwGetPrimaryMonitor();
	}

    void FullScreen(void) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(context, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    void Windowed(void) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(context, nullptr, 0, 0, (int)size.x, (int)size.y, mode->refreshRate);
    }

    void SetWiewPort(void) {
        int width, height;
        glfwGetFramebufferSize(context, &width, &height);
        glViewport(0, 0, width, height);
    }

    void enableGlParam(void) {
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    ~Window() {
        glfwDestroyWindow(context);
        glfwTerminate();
    }
};

#endif