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
/*
void Menu::toggleView() {
    if (status & DEBUG_VIEW) {
        status ^= DEBUG_VIEW;
        quad.Delete();
        quadShader.Delete();
        return;
    }

    status |= DEBUG_VIEW;
    quad.Init(glm::vec3(0.0f), glm::vec3(0.8f));
    quadShader.Load("shaders/debugTextureVS.glsl", "shaders/debugTextureFS.glsl");
}*/

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

void Menu::DrawViews() {
    glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(-0.9f, 0.1f, 0));

    glDisable(GL_DEPTH_TEST);
    quadShader.Activate();
    quadShader.setInt("depthMap", 3);
    quadShader.setInt("index", 0);
    quadShader.setMat4("matrix", matrix);
    quad.Render();
    quadShader.setInt("index", 1);
    quadShader.setMat4("matrix", glm::translate(matrix, glm::vec3(1.0f, 0, 0)));
    quad.Render();
    quadShader.setInt("index", 2);
    quadShader.setMat4("matrix", glm::translate(matrix, glm::vec3(0, -1.0f, 0)));
    quad.Render();
    glEnable(GL_DEPTH_TEST);
}

void Menu::DrawInfo() {
    ImGui::Begin("Minecraft Info", &showInfo, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    ImGui::SeparatorText("Player");
    ImGui::Text("Posision    x %5.2f, y %5.2f, z %5.2f", player->position.x, player->position.y, player->position.z);
    ImGui::Text("Target      x %5d, y %5d, z %5d  type %d", player->selectedCube.position.x, player->selectedCube.position.y, player->selectedCube.position.z, player->selectedCube.id);

    ImGui::NewLine();
    ImGui::SeparatorText("Chunk");
    ImGui::Text("Coordinate  x %3d, z %3d", (int)(player->position.x) >> 4, (int)(player->position.z) >> 4);
    
    ImGui::NewLine();
    ImGui::SeparatorText("Render");
    ImGui::Text("Minecraft average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
    ImGui::End();
}

void Menu::DrawMenu() {
    ImVec2 buttonSize;
    float  volume;
    static bool vsync = false;
    static bool shadowActive = true;
    static bool fullScreen = false;
    static bool wireFrameMode = false;
    static int renderDistance = RENDER_DISTANCE;

    ImGui::Begin("Menu");
    ImGui::SetWindowFocus();
    buttonSize.x = ImGui::GetWindowWidth() - 2 * ImGui::GetStyle().WindowPadding.x;
    buttonSize.y = 0;

  //  ImGui::SeparatorText("Options");
   // if (ImGui::CollapsingHeader("Options")) {
        ImGui::PushItemWidth(buttonSize.x);
        if (ImGui::SliderFloat("##FOV", &FOV, 20.0f, 160.0f, "FOV %.2f")) {
            player->camera.ChangePerspective(FOV, 0, 0, 0.1f, 0);
            updatePerspectiveCallback();
        }

        volume = 0.0f;
        ImGui::SliderFloat("##Volume", &volume, 0.0f, 100.0f, "Volume %.2f");

        ImGui::SliderFloat("##Mouse sensitivity", mouseSensitivity, 0.0f, 1.0f, "Mouse sensitivity %.2f");

        if (ImGui::SliderInt("##Render distance", &renderDistance, 8, 64, "Render distance %d")) {
            worldArea->UpdateRenderDistance(renderDistance);
            player->camera.ChangePerspective(0, 0, 0, 0.1f, 24.0f * renderDistance);
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

    if (ImGui::Button("Info", buttonSize))
        showInfo = !showInfo;

    if (ImGui::Button("Resume", buttonSize))
        Close();

    if (ImGui::Button("Exit", buttonSize))
        onExitCallback();

    ImGui::End();
}


void Menu::Draw() {

    if (!menuIsOpen && !showInfo) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showInfo)
        DrawInfo();

    if (menuIsOpen)
        DrawMenu();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    /*
    if (status & DEBUG_VIEW) {
        DrawViews();
        return;
    }*/
}