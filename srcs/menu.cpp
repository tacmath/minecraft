#include "menu.h"

Menu::Menu() {
    showInfo = true;
    menuIsOpen = false;
    mouseSensitivity = 0;
    FOV = 80;
    onExitCallback = [](void){};
    updatePerspectiveCallback = [](void){};
}

Menu::~Menu() {
    Delete();
}

void Menu::SetOnExitCallback(std::function<void(void)> onExitCallback) {
    this->onExitCallback = onExitCallback;
}

void Menu::SetUpdatPerspectiveCallback(std::function<void(void)> updatePerspectiveCallback) {
    this->updatePerspectiveCallback = updatePerspectiveCallback;
}

void Menu::SetupImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6;
    style.SeparatorTextAlign.x = 0.5f;
    style.WindowTitleAlign.x = 0.5f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    
    ImGui_ImplGlfw_InitForOpenGL(window->context, false);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Menu::Delete() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Menu::Link(Player *player, Window *window, WorldArea* worldArea, Shadow* shadow, float *mouseSensitivity) {
    this->player = player;
    this->window = window;
    this->worldArea = worldArea;
    this->shadow = shadow;
    this->mouseSensitivity = mouseSensitivity;
    FOV = player->camera.GetFOV();
    SetupImgui();
}

bool Menu::IsOpen() {
    return menuIsOpen;
}

void Menu::Open() {
    int x, y;

    menuIsOpen = true;
    glfwSetInputMode(window->context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetWindowSize(window->context, &x, &y);
    glfwSetCursorPos(window->context, x / 2.0, y / 2.0);
}

void Menu::Close() {
    menuIsOpen = false;
    glfwSetInputMode(window->context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Menu::Toogle() {
    if (menuIsOpen)
        Close();
    else
        Open();
}

void Menu::fpsTitle(float time, float latence) {
    static float previousUpdateTime_title = 0;
    static float diff = 0;
    static int frame_title = 0;
    char title[100];

    frame_title += 1;
    diff = time - previousUpdateTime_title;
    if (diff >= 0.2f) {
        sprintf(title, "Minecraft :  FPS: %d (%.3f ms)", frame_title * 5, latence * 1000);
        glfwSetWindowTitle(window->context, title);
        previousUpdateTime_title = time - (diff - 0.2f);
        frame_title = 0;
    }
}

void Menu::DrawInfo() {
    ImGui::Begin("Minecraft Info", &showInfo, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::SeparatorText("Player");
    ImGui::Text("Posision    x %5.2f, y %5.2f, z %5.2f", player->position.x, player->position.y, player->position.z);
    ImGui::Text("Velocity    x %5.2f, y %5.2f, z %5.2f", player->velocity.x, player->velocity.y, player->velocity.z);
    ImGui::Text("Target      x %5d, y %5d, z %5d  type %d   ", player->selectedCube.position.x, player->selectedCube.position.y, player->selectedCube.position.z, player->selectedCube.id);
    ImGui::Text("Grounded    %s", player->Grounded() ? "true" : "false");

    ImGui::NewLine();
    ImGui::SeparatorText("Chunk");
    ImGui::Text("Coordinate  x %3d, z %3d", (int)(player->position.x) >> 4, (int)(player->position.z) >> 4);
    
    ImGui::NewLine();
    ImGui::SeparatorText("Render");
    ImGui::Text("Minecraft average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    #if RENDERING_INFO == 1
        ImGui::Text("Nb Draw call  %d", mc_info.nb_drawcall);
        ImGui::Text("Polygon count %d", mc_info.polygon_count);
        mc_info.nb_drawcall = 0;
        mc_info.polygon_count = 0;
    #endif
    
    ImGui::End();
}

void Menu::DrawMenu() {
    ImVec2 buttonSize;
    static float  volume = 30.0f;
    static bool vsync = true;
    static bool shadowActive = true;
    static bool fullScreen = false;
    static bool wireFrameMode = false;
    static int renderDistance = RENDER_DISTANCE;

    ImGui::Begin("Menu", 0, ImGuiWindowFlags_NoScrollbar);
    buttonSize.x = ImGui::GetWindowWidth() - 2 * ImGui::GetStyle().WindowPadding.x;
    buttonSize.y = 0;

  //  ImGui::SeparatorText("Options");
   // if (ImGui::CollapsingHeader("Options")) {
        ImGui::PushItemWidth(buttonSize.x);
        if (ImGui::SliderFloat("##FOV", &FOV, 20.0f, 160.0f, "FOV %.2f")) {
            player->camera.ChangePerspective(FOV, 0, 0, 0.1f, 0);
            updatePerspectiveCallback();
        }

        if (ImGui::SliderFloat("##Volume", &volume, 0.0f, 100.0f, "Volume %.2f"))
            alListenerf(AL_GAIN, volume / 30.0f);

        ImGui::SliderFloat("##Mouse sensitivity", mouseSensitivity, 0.0f, 1.0f, "Mouse sensitivity %.2f");

        if (ImGui::SliderInt("##Render distance", &renderDistance, 8, 64, "Render distance %d")) {
            worldArea->UpdateRenderDistance(renderDistance);
            player->camera.ChangePerspective(0, 0, 0, 0.1f, 24.0f * renderDistance + 50);
            updatePerspectiveCallback();
        }

    //    ImGui::SliderInt("##FPS", &renderDistance, 30, 3000, "FPS %d");

        ImGui::PopItemWidth();

        if (ImGui::Checkbox("Shadow", &shadowActive)) {
            std::vector<std::string> shaderOption;
            if (shadowActive) {
                shaderOption.push_back("SHADOW");
                shadow->Activate();
            }
            else
                shadow->Delete();
            worldArea->ReloadShader(wireFrameMode, shaderOption);
            worldArea->initUniforms(player->camera);
        }
        
        ImGui::SameLine(); if (ImGui::Checkbox("Wireframe", &wireFrameMode)) {
            std::vector<std::string> shaderOption;
            if (shadowActive)
                shaderOption.push_back("SHADOW");
            worldArea->ReloadShader(wireFrameMode, shaderOption);
            worldArea->initUniforms(player->camera);
        }
        
        ImGui::SameLine(); if (ImGui::Checkbox("Fullscreen", &fullScreen)) {
            fullScreen ? window->FullScreen() : window->Windowed();
            glfwSwapInterval((vsync ? 1 : 0));
        }

        ImGui::SameLine(); ImGui::Checkbox("Collisions", &player->hasCollision);

        if (ImGui::Checkbox("Vsync", &vsync))
            glfwSwapInterval((vsync ? 1 : 0));

        ImGui::NewLine();
  //  }
     ON_DEBUG(debugWindows.DrawMenu();)

    if (ImGui::Button("Info", buttonSize))
        showInfo = !showInfo;

    if (ImGui::Button("Resume", buttonSize))
        Close();

    if (ImGui::Button("Exit", buttonSize))
        onExitCallback();

    ImGui::End();
}


void Menu::Draw() {
    if (!menuIsOpen && !showInfo ON_DEBUG(&& !debugWindows.IsActive())) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showInfo)
        DrawInfo();

    if (menuIsOpen)
        DrawMenu();

    ON_DEBUG(debugWindows.Draw(window->context);)

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}