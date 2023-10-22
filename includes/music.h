#pragma once

#include "soundBuffer.h"
#include "soundSource.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <iomanip>
#include <thread>

class Music {

    ALuint          source;
    SoundBuffers    buffers;

public:
    Music();
    ~Music();

    void Add(const std::string& Filename);
    void Add(ALuint buffer);
    void LoadPlayList(const std::vector<std::string>& Filenames);
    void Play();
    void Stop();
    void Pause();
    ALint Status();
    void Delete();
};
