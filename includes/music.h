#pragma once

#include "soundBuffer.h"
#include "soundSource.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <iomanip>
#include <thread>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <random>

#define MUSIC_PATH "sound/music"

class Music {

    ALuint          source;
    SoundBuffers    buffers;

    std::vector<std::string> GetMusicFiles();

public:
    Music();
    ~Music();

    void Add(const std::string& Filename);
    void Add(ALuint buffer);
    void LoadPlayList(std::vector<std::string>& Filenames);
    void Play();
    void Stop();
    void Pause();
    ALint Status();
    void Delete();
};
