#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "camera.h"
#include "quad.h"
#include "shader.h"

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

public:
    

    Menu();
    
    ~Menu();

    void Link(Player *player, GLFWwindow  *window);

    void Open();

    void Toogle();

    void Draw();

    void fpsTitle(float time, float latence);
    
private:
    void SetupImgui();

    void DrawViews();

    void DrawInfo();

    void DrawMenu();

    void Delete();
};

#endif