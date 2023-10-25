#include "music.h"
#include <mutex>

static std::mutex music_mtx;


static void loadMusicFile(const std::string& Filename, Music* music) {
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
        return;
    ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);
    // Lecture des echantillons audio au format entier 16 bits signe (le plus commun)
    std::vector<ALshort> Samples(NbSamples);
    if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
        return;
    // Fermeture du fichier
    sf_close(File);
    ALenum Format;
    switch (FileInfos.channels)
    {
    case 1:  Format = AL_FORMAT_MONO16; break;
    case 2:  Format = AL_FORMAT_STEREO16; break;
    default: return;
    }

    // Creation du tampon OpenAL
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    // Remplissage avec les echantillons lus
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    music_mtx.lock();
    music->Add(buffer);
    music_mtx.unlock();
}



Music::Music() {
    alGenSources(1, &source);
    LoadPlayList({ "sound/hall14", "sound/piano2", "sound/hall13", "sound/piano3" });
    Play();
}

Music::~Music() {
    Delete();
}

void Music::Add(const std::string& Filename) {
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
        return;
    ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);
    // Lecture des echantillons audio au format entier 16 bits signe (le plus commun)
    std::vector<ALshort> Samples(NbSamples);
    if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
        return;
    // Fermeture du fichier
    sf_close(File);
    ALenum Format;
    switch (FileInfos.channels)
    {
    case 1:  Format = AL_FORMAT_MONO16; break;
    case 2:  Format = AL_FORMAT_STEREO16; break;
    default: return;
    }

    // Creation du tampon OpenAL
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    // Remplissage avec les echantillons lus
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    buffers.Add(buffer);
    alSourceQueueBuffers(source, 1, &buffer);
}

void Music::Add(ALuint buffer) {
    buffers.Add(buffer);
    alSourceQueueBuffers(source, 1, &buffer);
}

void Music::LoadPlayList(const std::vector<std::string>& Filenames) { // try to use std::async, std::future pour charger des gros assest et utiliser std::for_eatch a des endrois repetitif

    std::vector<std::thread> threads;

    buffers.resize(Filenames.size());
    threads.resize(Filenames.size());
    for (unsigned n = 0; n < threads.size(); ++n)
        threads[n] = std::thread(loadMusicFile, Filenames[n], this);

    for (auto& th : threads) th.join();
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