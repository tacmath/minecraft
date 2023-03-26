#include "menu.h"

Menu::Menu() {
    showInfo = true;
    menuIsOpen = false;
    onExitCallback = [](void){};
}

Menu::~Menu() {
    Delete();
}

void Menu::SetOnExitCallback(std::function<void(void)> onExitCallback) {
    this->onExitCallback = onExitCallback;
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
    
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Menu::Delete() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Menu::Link(Player *player, GLFWwindow *window) {
    this->player = player;
    this->window = window;
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetWindowSize(window, &x, &y);
    glfwSetCursorPos(window, x / 2.0, y / 2.0);
}

void Menu::Close() {
    menuIsOpen = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
        glfwSetWindowTitle(window, title);
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
    static float f = 0.0f;
    static int counter = 0;
    bool test;

    ImGui::Begin("Minecraft Info", &showInfo, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    ImGui::SeparatorText("Player");
    ImGui::Text("Posision    x %5.2f, y %5.2f, z %5.2f", player->position.x, player->position.y, player->position.z);
    ImGui::Text("Target      x %5d, y %5d, z %5d  type %d", player->selectedCube.position.x, player->selectedCube.position.y, player->selectedCube.position.z, player->selectedCube.id);

    ImGui::NewLine();
    ImGui::SeparatorText("Chunk");
    ImGui::Text("Coordinate  x %3d, z %3d", (int)(player->position.x) >> 4, (int)(player->position.z) >> 4);
    
    ImGui::NewLine();
    ImGui::Text("Minecraft average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
    ImGui::End();
}

void Menu::DrawMenu() {
    ImVec2 buttonSize;

    ImGui::Begin("Menu");
    ImGui::SetWindowFocus();
    buttonSize.x = ImGui::GetWindowWidth() - 2 * ImGui::GetStyle().WindowPadding.x;
    buttonSize.y = 0;

    ImGui::SeparatorText("Options");

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