#pragma once

#include <iostream>
#include <AL/alc.h>
#include <AL/al.h>
#include <vector>
#include <sndfile.h>
#include <iomanip>
#include <thread>

class Listener
{
public:
    Listener();
    ~Listener();

    static void SetVolume(float volume);

    static void SetPosition(float x, float y, float z);

    static void SetDirection(float x, float y, float z);
};

class SoundBuffer
{
public:
    ALuint ID;

    SoundBuffer();

    void Load(const char* file);

    void Delete();
};

class SoundSource
{
    ALuint ID;
public:
    SoundSource() {
        ID = 0;
    }

    void Gen() {
        alGenSources(1, &ID);
    }

    void Play(ALuint bufferID) {
        Stop();
        alSourcei(ID, AL_BUFFER, bufferID);
        Resume();
    }

    void Play(SoundBuffer& buffer) {
        Stop();
        alSourcei(ID, AL_BUFFER, buffer.ID);
        Resume();
    }

    void Stop() {
        alSourceStop(ID);
    }

    void Pause() {
        alSourcePause(ID);
    }

    void Resume() {
        alSourcePlay(ID);
    }

    void Loop(bool loop) {
        alSourcei(ID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }
    bool IsPlaying() {
        ALint status;
        alGetSourcei(ID, AL_SOURCE_STATE, &status);
        return status == AL_PLAYING;
    }


    void Delete() {
        if (ID)
            alDeleteSources(1, &ID);
        ID = 0;
    }

    void SetVolume(float volume) {
        alSourcef(ID, AL_GAIN, volume);
    }

    void SetPitch(float pitch) {
        alSourcef(ID, AL_PITCH, pitch);
    }

    void SetPosition(float x, float y, float z)
    {
        alSource3f(ID, AL_POSITION, x, y, z);
    }

    void SetDirection(float x, float y, float z)
    {
        ALfloat Orientation[] = { x, y, z, 0.f, 1.f, 0.f };
        alSourcefv(ID, AL_ORIENTATION, Orientation);
    }
};

class Music {

    ALuint source;
    std::vector<ALuint> buffers;

public:
    Music();
    ~Music();

    void Load(const std::string& Filename);
    void Play();
    void Stop();
    void Pause();
    ALint Status();
    void Delete();
};

void InitMusic(Music& music);