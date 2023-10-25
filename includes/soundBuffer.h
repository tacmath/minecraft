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


class SoundBuffers : public std::vector<SoundBuffer> {
    using std::vector<SoundBuffer>::vector;

public:

    void Add(const char* file) {
        SoundBuffer buffer;

        buffer.Load(file);
        if (buffer.ID > 0)
            this->push_back(buffer);
    }

    void Add(const SoundBuffer buffer) {
        this->push_back(buffer);
    }

    void Add(ALuint ID) {
        SoundBuffer buffer;

        buffer.ID = ID;
        this->push_back(buffer);
    }

    // create a sub SoundBuffers that copy of all SoundBuffer id
    // ! do not do a Clear on the copy they it share the same buffer ids of the original
    SoundBuffers Sub(size_t offset, size_t size) const {
        if (offset >= this->size() || !size)
            return SoundBuffers();
        if (offset + size >= this->size())
            size = this->size() - offset - 1;
        return SoundBuffers(this->begin() + offset, this->begin() + offset + size);
    }

    // get a random sound buffer or return -1 if no valid buffer is found
    ALuint GetRandom() const {
        if (this->size() < 1)
            return -1;
        return (*this)[0].ID;
    }

    // clear the list and delete all soundBuffer
    void Clear() {
        for (SoundBuffer buffer : *this)
            buffer.Delete();
        this->clear();
    }
};
//typedef std::vector<SoundBuffer> SoundBuffers;