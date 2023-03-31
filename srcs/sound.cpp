#include "sound.h"
#include <mutex>

static std::mutex music_mtx;

Listener::Listener() {
    ALCdevice* Device = alcOpenDevice(NULL);
    if (!Device)
        return;

    // Cr�ation du contexte
    ALCcontext* Context = alcCreateContext(Device, NULL);
    if (!Context)
        return;

    // Activation du contexte
    if (!alcMakeContextCurrent(Context))
        return;

    alDistanceModel(AL_INVERSE_DISTANCE);
}

Listener::~Listener() {
    ALCcontext* Context = alcGetCurrentContext();
    ALCdevice* Device = alcGetContextsDevice(Context);

    // D�sactivation du contexte
    alcMakeContextCurrent(NULL);

    // Destruction du contexte
    alcDestroyContext(Context);

    // Fermeture du device
    alcCloseDevice(Device);
}

void Listener::SetVolume(float volume) {
    alListenerf(AL_GAIN, volume);
}

void Listener::SetPosition(float x, float y, float z)
{
    alListener3f(AL_POSITION, x, y, z);
}

void Listener::SetDirection(float x, float y, float z)
{
    ALfloat Orientation[] = { x, y, z, 0.f, 1.f, 0.f };
    alListenerfv(AL_ORIENTATION, Orientation);
}

SoundBuffer::SoundBuffer()
{
    ID = 0;
}

void SoundBuffer::Load(const char* file)
{
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(file, SFM_READ, &FileInfos);
    if (!File)
        return;
    ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);
    // Lecture des �chantillons audio au format entier 16 bits sign� (le plus commun)
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

    // Cr�ation du tampon OpenAL

    if (!ID)
        alGenBuffers(1, &ID);
    // Remplissage avec les �chantillons lus
    alBufferData(ID, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);
}

void SoundBuffer::Delete()
{
    if (ID)
        alDeleteBuffers(1, &ID);
    ID = 0;
}


static void loadMusicFile(const std::string& Filename, Music *music) {
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
        return;
    ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);
    // Lecture des �chantillons audio au format entier 16 bits sign� (le plus commun)
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

    // Cr�ation du tampon OpenAL
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    // Remplissage avec les �chantillons lus
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    music_mtx.lock();
    music->Add(buffer);
    music_mtx.unlock();
}



Music::Music() {
    alGenSources(1, &source);
    LoadPlayList({"sound/hall14", "sound/piano2", "sound/hall13", "sound/piano3"});
    Play();
}

Music::~Music() {
}

void Music::Add(const std::string& Filename) {
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
        return;
    ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);
    // Lecture des �chantillons audio au format entier 16 bits sign� (le plus commun)
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

    // Cr�ation du tampon OpenAL
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    // Remplissage avec les �chantillons lus
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    buffers.push_back(buffer);
    alSourceQueueBuffers(source, 1, &buffer);
}

void Music::Add(ALuint buffer) {
    buffers.push_back(buffer);
    alSourceQueueBuffers(source, 1, &buffer);
}

void Music::LoadPlayList(const std::vector<std::string>& Filenames) { // try to use std::async, std::future pour charger des gros assest et utiliser std::for_eatch a des endrois r�p�titif

    std::vector<std::thread> threads;

    buffers.reserve(Filenames.size());
    threads.resize(Filenames.size());
    for (int n = 0; n < threads.size(); ++n)
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

    for (auto buffer : buffers)
        alDeleteBuffers(1, &buffer);
    buffers.clear();
    source = 0;
}
