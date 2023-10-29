#pragma once

#include <AL/alc.h>
#include <AL/al.h>
#include "soundBuffer.h"
#include <array>

#define MAX_SOUND_SOURCES 10

class SoundSource
{
public:
    ALuint ID;

    SoundSource();

    void Gen();
    void Delete();

    void Play(ALuint bufferID);
    void Play(const SoundBuffer& buffer);

    void PlayRandom(const SoundBuffers& buffers) {
        Play(buffers.GetRandom());
    }

    void Stop();
    void Pause();
    void Resume();
    void Loop(bool loop);

    bool IsPlaying() const;

    void SetVolume(float volume);
    void SetPitch(float pitch);
    void SetType(ALint type);
    void SetRolloffFactor(float rolloffFactor);
    void SetPosition(float x, float y, float z);
    void SetDirection(float x, float y, float z);
};


template <size_t _size>
class SoundSources : private std::array<SoundSource, _size> {
public:
    SoundSources() {
        for (size_t n = 0; n < _size; n++) {
            (*this)[n].Gen();
            //  (*this)[n].SetRolloffFactor(0.5f);
        }
    }

    void Delete() {
        for (size_t n = 0; n < _size; n++)
            (*this)[n].Delete();
    }

    SoundSource GetSoundSource() const {
        size_t n = 0;

        while (n < (_size - 1) && (*this)[n].IsPlaying())
            n++;
        return (*this)[n];
    }
};