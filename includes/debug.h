#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "player.h"
#include "camera.h"
#include "textDisplay.h"

class Debug {
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

    void Init(glm::vec2 *windowSize, Player *player, Camera *camera);

    void setProjection(float width, float height);

    void enable();

    void disable();

    void toggle();

    void Draw(float time, float latence, Minecraft &minecraft);

    void fpsTitle(float time, float latence, Minecraft& minecraft);
};

#endif