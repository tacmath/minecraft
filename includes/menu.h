#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "camera.h"
#include "quad.h"
#include "shader.h"

#include <functional>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

class Menu {
    GLFWwindow  *window;
    Camera      *camera;
    Player      *player;

    Shader		quadShader;
    Quad        quad;

    bool        showInfo;
    bool        menuIsOpen;

    std::function<void(void)> onExitCallback;

public:

    Menu();
    
    ~Menu();

    void Link(Player *player, GLFWwindow  *window);

    void Open();

    bool IsOpen();

    void Toogle();

    void Draw();

    void fpsTitle(float time, float latence);

    void SetOnExitCallback(std::function<void(void)> onExitCallback);
    
private:
    void SetupImgui();

    void DrawViews();

    void DrawInfo();

    void DrawMenu();

    void Close();

    void Delete();
};

#endif