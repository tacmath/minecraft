#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "camera.h"
#include "textDisplay.h"

# define MAX_FPS        4000

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

    TextDisplay text;
    bool visible;


public:

    Debug();

    void Init(glm::vec2 *windowSize, Player *player, Camera *camera, GLFWwindow  *window);

    void setProjection(float width, float height);

    void enable();

    void disable();

    void toggle();

    void Draw(float time, float latence);

    void fpsTitle(float time, float latence);
};

#endif