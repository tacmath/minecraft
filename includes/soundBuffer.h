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
    void Add(const char* file) {
        SoundBuffer buffer;

        buffer.Load(file);
        this->push_back(buffer);
    }

    void Add(SoundBuffer buffer) {
        this->push_back(buffer);
    }

    void Add(ALuint ID) {
        SoundBuffer buffer;

        buffer.ID = ID;
        this->push_back(buffer);
    }

    ALuint GetRandom() const {
        if (this->size() < 1)
            return -1;
        return (*this)[0].ID;
    }

    void Reserve(const size_t size) {
        this->reserve(size);
    }

    void Clear() {
        for (SoundBuffer buffer : *this)
            buffer.Delete();
        this->clear();
    }
};
//typedef std::vector<SoundBuffer> SoundBuffers;