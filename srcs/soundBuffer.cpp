#include "soundBuffer.h"

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

    if (!ID)
        alGenBuffers(1, &ID);
    // Remplissage avec les echantillons lus
    alBufferData(ID, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);
}

void SoundBuffer::Delete()
{
    if (ID)
        alDeleteBuffers(1, &ID);
    ID = 0;
}