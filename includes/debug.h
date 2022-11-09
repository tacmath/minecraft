#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include "minecraft.h"
#include "textDisplay.h"

class Debug {
    int frame;
    float previousUpdateTime;
    float diff;
    char fps[100];
    char xyz[100];
    TextDisplay text;
    bool visible;


public:

	Debug();

    void enable();

    void disable();

    void display(float time, float latence, Minecraft &minecraft);
};

#endif