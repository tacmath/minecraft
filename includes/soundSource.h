#pragma once

#include <AL/alc.h>
#include <AL/al.h>
#include "soundBuffer.h"

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

    bool IsPlaying();

    void SetVolume(float volume);
    void SetPitch(float pitch);
    void SetType(ALint type);
    void SetRolloffFactor(float rolloffFactor);
    void SetPosition(float x, float y, float z);
    void SetDirection(float x, float y, float z);
};


template <size_t _size>
class SoundSources : std::array<SoundSource, _size> {
public:
    SoundSources();
    ~SoundSources();
    SoundSource GetSoundSource();
};

/*
namespace sound {
    SoundSources<MAX_SOUND_SOURCES> sources;
}*/