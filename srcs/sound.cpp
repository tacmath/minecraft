#include "sound.h"

Listener::Listener() {
    ALCdevice* Device = alcOpenDevice(NULL);
    if (!Device)
        return;

    // Création du contexte
    ALCcontext* Context = alcCreateContext(Device, NULL);
    if (!Context)
        return;

    // Activation du contexte
    if (!alcMakeContextCurrent(Context))
        return;
}

Listener::~Listener() {
    ALCcontext* Context = alcGetCurrentContext();
    ALCdevice* Device = alcGetContextsDevice(Context);

    // Désactivation du contexte
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
    // Lecture des échantillons audio au format entier 16 bits signé (le plus commun)
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

    // Création du tampon OpenAL

    if (!ID)
        alGenBuffers(1, &ID);
    // Remplissage avec les échantillons lus
    alBufferData(ID, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);
}

void SoundBuffer::Delete()
{
    if (ID)
        alDeleteBuffers(1, &ID);
    ID = 0;
}


Music::Music() {
    source = 0;
}

Music::~Music() {
}

void Music::Load(const std::string& Filename) {
    // Ouverture du fichier audio avec libsndfile
    if (!source) {
        alGenSources(1, &source);
    }
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
        return;
    ALsizei NbSamples = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);
    // Lecture des échantillons audio au format entier 16 bits signé (le plus commun)
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

    // Création du tampon OpenAL
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    // Remplissage avec les échantillons lus
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    buffers.push_back(buffer);
    alSourceQueueBuffers(source, 1, &buffer);
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
    if (source) {
        alSourceStop(source);
        alSourcei(source, AL_BUFFER, 0);
        alDeleteSources(1, &source);
    }
    for (auto buffer : buffers)
        alDeleteBuffers(1, &buffer);
    buffers.clear();
    source = 0;
}

static void loadMusic(Music& music) { //need to upgade the system it can crash
    music.Load("sound/piano1");
    music.Load("sound/hall13");
    music.Load("sound/piano2");
    music.Load("sound/hall14");
    music.Load("sound/piano3");
    music.Play();
}

void InitMusic(Music& music) {
    std::thread(loadMusic, std::ref(music)).detach();
}

