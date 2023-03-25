#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "camera.h"
#include "textDisplay.h"
#include "quad.h"
#include "shader.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

# define MAX_FPS        6000

enum DEBUG { DEBUG_OFF, DEBUG_ON, DEBUG_VIEW};

class Menu {
    GLFWwindow  *window;
    Camera      *camera;
    Player      *player;
    glm::vec2   *windowSize;

    int frame;
    int frame_title;
    float previousUpdateTime;
    float previousUpdateTime_title;
    float diff;

    Shader		quadShader;
    Quad        quad;

    TextDisplay text;

public:

    int status;

    Menu();
    
    ~Menu();

    void Link(glm::vec2 *windowSize, Player *player, GLFWwindow  *window);

    void setProjection(float width, float height);

    void enable();

    void disable();

    void toggle();

    void toggleView();

    void Draw(float time, float latence);
    
    void DrawViews();

    void fpsTitle(float time, float latence);

    void Delete();

private:
    void SetupImgui();
};

#endif