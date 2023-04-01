#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "quad.h"
#include "shader.h"
#include "window.h"
#include "shadow.h"
#include "world_area.h"
#include "global_defines.h"
#include "AL/al.h"

#include <functional>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

#if DEBUG_MODE == 1

class DebugWindow {
    enum { OPEN, CLOSED }       status;
    std::function<void(void)>   renderFun;
    GLuint                      textureID;

public:
    const char* name;

    DebugWindow() {
        textureID = 0;
        renderFun = [](void) {};
        name = "unamed";
        status = CLOSED;
    }
    void Init(const char* name, const std::function<void(void)>& renderFun) {
        this->name = name;
        this->renderFun = renderFun;
    }

    void Open() {
        if (textureID == 0) {
            glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
            glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureStorage2D(textureID, 1, GL_RGB8, 512, 512);
        }
        status = OPEN;
    }

    void Close() {
        if (textureID)
            glDeleteTextures(1, &textureID);
        textureID = 0;
        status = CLOSED;
    }

    bool Draw() { // return true when closed
        ImVec2 max;
        bool isOpen = true;

        if (status != OPEN)
            return false;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        
        renderFun();

        ImGui::Begin(name, &isOpen, ImGuiWindowFlags_NoCollapse);
        if (!isOpen) {
            ImGui::End();
            Close();
            return true;
        }
        max = ImGui::GetWindowContentRegionMax();
        max.y -= ImGui::GetFontSize() + 6;
        ImGui::Image((void*)(intptr_t)textureID, max);
        ImGui::End();
        return false;
    }
};

#define MAX_DEBUG_WINDOWS 3

class DebugWindows {
    Shader		shader;
    Quad        quad;
    DebugWindow window[MAX_DEBUG_WINDOWS];
    GLuint      frameBufferID;
    bool        open[MAX_DEBUG_WINDOWS];
    unsigned    nbOpen;
public:

    DebugWindows() {
        nbOpen = 0;
        for (unsigned n = 0; n < MAX_DEBUG_WINDOWS; n++)
            open[n] = false;
            
        window[0].Init("Shadow map 1", [&](void) {
            shader.Activate();
            shader.setInt("index", 0);
            quad.Render();
        });

        window[1].Init("Shadow map 2", [&](void) {
            shader.Activate();
            shader.setInt("index", 1);
            quad.Render();
        });

        window[2].Init("Shadow map 3", [&](void) {
            shader.Activate();
            shader.setInt("index", 2);
            quad.Render();
        });
    }

    ~DebugWindows() {
        if (nbOpen)
            Destroy();
    }

    void DrawMenu() {
        if (ImGui::CollapsingHeader("Debug views")) {
            for (unsigned n = 0; n < MAX_DEBUG_WINDOWS; n++) {
                if ((n % 3))
                    ImGui::SameLine();
                if (ImGui::Checkbox(window[n].name, &open[n])) {
                    if (open[n]) {
                        window[n].Open();
                        nbOpen++;
                        if (nbOpen == 1)
                            Init();
                    }
                    else {
                        window[n].Close();
                        nbOpen--;
                        if (!nbOpen)
                            Destroy();
                    }
                }
            }
        }

       /* if (ImGui::Checkbox("All views", )) {
            for (unsigned n = 0; n < MAX_DEBUG_WINDOWS; n++) {
                if (!open[n]) {
                    open[n] = true;
                    window[n].Open();
                    nbOpen++;
                    if (nbOpen == 1)
                        Init();
                }
            }
        }*/
    }

    void Draw(GLFWwindow *context) {
        if (!nbOpen) return;

        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        glViewport(0, 0, 512, 512);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

        for (unsigned n = 0; n < MAX_DEBUG_WINDOWS; n++)
            if (window[n].Draw()) {
                open[n] = false;
                nbOpen--;
            }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int width, height;
        glfwGetFramebufferSize(context, &width, &height);    // reset the viewport
        glViewport(0, 0, width, height);

        if (!nbOpen)
            Destroy();
    }

    bool IsActive() {
        return nbOpen > 0;
    }

private:
    void Init() {
        quad.Init();
        shader.Load("shaders/debugTextureVS.glsl", "shaders/debugTextureFS.glsl");
        shader.setInt("depthMap", 3);
        shader.setMat4("matrix", glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        glGenFramebuffers(1, &frameBufferID);
    }

    void Destroy() {
        quad.Delete();
        shader.Delete();
        glDeleteFramebuffers(1, &frameBufferID);
        frameBufferID = 0;
        for (unsigned n = 0; n < MAX_DEBUG_WINDOWS; n++)
            window[0].Close();
    }
};

#endif // DEBUG_MODE

class Menu {
    Window      *window;
    Player      *player;
    WorldArea   *worldArea;
    Shadow      *shadow;

    ON_DEBUG(DebugWindows debugWindows;)
    
    float       *mouseSensitivity;
    float       FOV;

    bool        showInfo;
    bool        menuIsOpen;

    std::function<void(void)> onExitCallback;
    std::function<void(void)> updatePerspectiveCallback;

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

    void SetUpdatPerspectiveCallback(std::function<void(void)> updatePerspectiveCallback);
    
private:
    void SetupImgui();

    void DrawInfo();

    void DrawMenu();

    void Close();

    void Delete();
};

#endif