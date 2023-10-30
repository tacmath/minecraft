#pragma once

#include <AL/alc.h>
#include <AL/al.h>

#define NULL 0

class Listener
{
public:
    Listener();
    ~Listener();

    static void SetVolume(float volume);
    static void SetPosition(float x, float y, float z);
    static void SetDirection(float x, float y, float z);
};