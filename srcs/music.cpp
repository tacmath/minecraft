#include "music.h"
/*#include <mutex>

static std::mutex music_mtx;*/


static void loadMusicFile(const std::string& Filename, SoundBuffer* buffer) {
    buffer->Load(Filename.c_str());
}

static inline bool file_exists(const char* name) {
    struct stat buffer;
    return (stat(name, &buffer) == 0);
}

std::vector<std::string> Music::GetMusicFiles() {
    std::vector<std::string> musicFiles;

    if (!file_exists(MUSIC_PATH)) {
        std::cerr << "The music folder '" << MUSIC_PATH << "' does not exist" << std::endl;
        return musicFiles;
    }

    for (const auto& entry : std::filesystem::directory_iterator(MUSIC_PATH))
        musicFiles.push_back(entry.path().string());
    return musicFiles;
}

Music::Music() {
    std::vector<std::string> musicFiles = GetMusicFiles();
    
    alGenSources(1, &source);
    LoadPlayList(musicFiles);
    Play();
}

Music::~Music() {
    Delete();
}

void Music::Add(const std::string& Filename) {
    SoundBuffer buffer;

    buffer.Load(Filename.c_str());
    buffers.Add(buffer);
    alSourceQueueBuffers(source, 1, &buffer.ID);
}

void Music::Add(ALuint buffer) {
    buffers.Add(buffer);
    alSourceQueueBuffers(source, 1, &buffer);
}


void Music::LoadPlayList(const std::vector<std::string>& Filenames) { // try to use std::async, std::future pour charger des gros assest et utiliser std::for_eatch a des endrois repetitif
    std::vector<std::thread> threads;
    std::random_device random;
    std::mt19937 engine(random());

    buffers.resize(Filenames.size());
    threads.resize(Filenames.size());
    for (unsigned n = 0; n < threads.size(); ++n)
        threads[n] = std::thread(loadMusicFile, Filenames[n], &buffers[n]);

    for (auto& thread : threads) thread.join();

    std::shuffle(std::begin(buffers), std::end(buffers), engine);

    for (const SoundBuffer buffer : buffers)
        alSourceQueueBuffers(source, 1, &buffer.ID);
}

void Music::Play() {
    alSourcei(source, AL_LOOPING, AL_TRUE);
    alSourcePlay(source);
}

void Music::Stop() {
    alSourceStop(source);
}

void Music::Pause() {
    alSourcePause(source);
}

ALint Music::Status() {
    ALint status;
    alGetSourcei(source, AL_SOURCE_STATE, &status);
    return status;
}

void Music::Delete() {
    if (source)
        alDeleteSources(1, &source);
    source = 0;

    buffers.Clear();
}