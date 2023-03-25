#include "menu.h"

Menu::Menu() {
    diff = 0;
    frame = 0;
    status = DEBUG_OFF;
    previousUpdateTime = 0.0f;
}

Menu::~Menu() {
    Delete();
}

void Menu::SetupImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Menu::Delete() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Menu::Link(glm::vec2 *windowSize, Player *player, GLFWwindow *window) {
    this->windowSize = windowSize;
    this->player = player;
    this->window = window;
    text.Init(windowSize->x, windowSize->y);
    SetupImgui();
}

void Menu::toggle() {
    if (status & DEBUG_ON) {
        disable();
        return;
    }
    enable();
}

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
}

void Menu::enable() {
    status |= DEBUG_ON;
}

void Menu::disable() {
    status ^= DEBUG_ON;
    previousUpdateTime = 0;
    frame = 0;
}

void Menu::setProjection(float width, float height) {
    text.setProjection(width, height);
}

void Menu::fpsTitle(float time, float latence) {
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

void Menu::Draw(float time, float latence) {

    if (!(status & DEBUG_ON)) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    static float f = 0.0f;
    static int counter = 0;
    bool test;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &test);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &test);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (status & DEBUG_VIEW) {
        DrawViews();
        return;
    }
    /*
    frame += 1;
    diff = time - previousUpdateTime;
    if (diff >= 0.2f) {
        if (previousUpdateTime > 10.0) {
            sprintf(fps, "FPS: %d  (max %d / latence: %.3f ms)", frame * 5, MAX_FPS, latence * 1000.0f);
        }
        else {
            sprintf(fps, "FPS: calculate...");
        }
        frame = 0;
        if (diff >= 0.4) diff = 0.0f;
        previousUpdateTime = time - (diff - 0.2f);
    }
    sprintf(xyz, "XYZ: %.3f / %.3f / %.3f", player->position.x, player->position.y, player->position.z);


    Chunk *chunk = GetChunk((int)(player->position.x) >> 4, (int)(player->position.z) >> 4);
    if (chunk != 0) {
        sprintf(xz, "Chunk: %d %d", chunk->posx, chunk->posz);
    }
    else {
        sprintf(xz, "Chunk: undefined");
    }

    sprintf(target, "target: %d %d %d", player->selectedCube.position.x, player->selectedCube.position.y, player->selectedCube.position.z);

    float y = windowSize->y - 15;
    float scale = (y / 600) / 5;
    float space = scale * 100.0f;
    text.display("Vox Version 0.1", 5.0f, y, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(fps, 5.0f, y - space * 1, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xyz, 5.0f, y - space * 2, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(xz, 5.0f, y - space * 3, scale, glm::vec3(1.0, 1.0f, 1.0f));
    text.display(target, 5.0f, y - space * 4, scale, glm::vec3(1.0, 1.0f, 1.0f));*/
}