#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "camera.h"
#include "textDisplay.h"
#include "quad.h"
#include "shader.h"

# define MAX_FPS        6000

#define DEBUG_ON 1
#define DEBUG_OFF 0
#define DEBUG_VIEW 2

class Debug {
    GLFWwindow  *window;
    Camera      *camera;
    Player      *player;
    glm::vec2   *windowSize;

    int frame;
    int frame_title;
    float previousUpdateTime;
    float previousUpdateTime_title;
    float diff;
    char fps[100];
    char xyz[100];
    char xz[100];
    char target[100];

    Shader		quadShader;
    Shader		quadShader2;
    Quad        quad;

    TextDisplay text;

public:

    int status;

    Debug();

    void Link(glm::vec2 *windowSize, Player *player, GLFWwindow  *window);

    void setProjection(float width, float height);

    void enable();

    void disable();

    void toggle();

    void toggleView();

    void Draw(float time, float latence);
    
    void DrawViews();

    void fpsTitle(float time, float latence);
};

#endif