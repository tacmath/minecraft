#pragma once

#include <AL/alc.h>
#include <AL/al.h>
#include <vector>
#include <sndfile.h>
#include <random>

class SoundBuffer
{
public:
    ALuint ID;

    SoundBuffer();

    void Load(const char* file);

    void Delete();
};


class SoundBuffers : public std::vector<SoundBuffer> {
    using std::vector<SoundBuffer>::vector;

public:

    void Add(const char* file);
    void Add(const SoundBuffer buffer);
    void Add(ALuint ID);

    // create a sub SoundBuffers that copy of all SoundBuffer id
    // ! do not do a Clear on the copy they it share the same buffer ids of the original
    SoundBuffers Sub(size_t offset, size_t size) const;

    // get a random sound buffer or return -1 if no valid buffer is found
    ALuint GetRandom() const;

    // clear the list and delete all soundBuffer
    void Clear();
};
//typedef std::vector<SoundBuffer> SoundBuffers;