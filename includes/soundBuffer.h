#pragma once

#include <AL/alc.h>
#include <AL/al.h>
#include <vector>
#include <sndfile.h>

class SoundBuffer
{
public:
    ALuint ID;

    SoundBuffer();

    void Load(const char* file);

    void Delete();
};

class SoundBuffers : std::vector<SoundBuffer> {
public:
    ALuint GetRandom() const {
        if (this->size() < 1)
            return -1;
        return (*this)[0].ID;
    }
};
//typedef std::vector<SoundBuffer> SoundBuffers;