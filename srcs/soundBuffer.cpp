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

// get a random sound buffer or return -1 if no valid buffer is found

void SoundBuffers::Add(const char* file) {
    SoundBuffer buffer;

    buffer.Load(file);
    if (buffer.ID > 0)
        this->push_back(buffer);
}

void SoundBuffers::Add(const SoundBuffer buffer) {
    this->push_back(buffer);
}

void SoundBuffers::Add(ALuint ID) {
    SoundBuffer buffer;

    buffer.ID = ID;
    this->push_back(buffer);
}


// create a sub SoundBuffers that copy of all SoundBuffer id
// ! do not do a Clear on the copy they it share the same buffer ids of the original
SoundBuffers SoundBuffers::Sub(size_t offset, size_t size) const {
    if (offset >= this->size() || !size)
        return SoundBuffers();
    if (offset + size >= this->size())
        size = this->size() - offset - 1;
    return SoundBuffers(this->begin() + offset, this->begin() + offset + size);
}
#include <iostream>
ALuint SoundBuffers::GetRandom() const {
    if (this->size() < 1)
        return -1;
    std::random_device random; //maybe set as private var of SoundBuffers or static
    return (*this)[random() % this->size()].ID;
}

// clear the list and delete all soundBuffer

void SoundBuffers::Clear() {
    for (SoundBuffer buffer : *this)
        buffer.Delete();
    this->clear();
}
