#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "quad.h"
#include "shader.h"
#include "window.h"
#include "shadow.h"
#include "world_area.h"

#include <functional>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

class Menu {
    Window      *window;
    Player      *player;
    WorldArea   *worldArea;
    Shadow      *shadow;

    Shader		quadShader;
    Quad        quad;
    
    float       *mouseSensitivity;
    float       FOV;

    bool        showInfo;
    bool        menuIsOpen;

    std::function<void(void)> onExitCallback;
    std::function<void(float)> updateFOVCallback;

public:

    Menu();
    
    ~Menu();

    void Link(Player *player, Window *window, WorldArea* worldArea, Shadow* shadow, float* mouseSensitivity);

    void Open();

    bool IsOpen();

    void Toogle();

    void Draw();

    void fpsTitle(float time, float latence);

    void SetOnExitCallback(std::function<void(void)> onExitCallback);

    void SetUpdateFOVCallback(std::function<void(float)> updateFOVCallback);
    
private:
    void SetupImgui();

    void DrawViews();

    void DrawInfo();

    void DrawMenu();

    void Close();

    void Delete();
};

#endif