#include "soundSource.h"

SoundSource::SoundSource() {
    ID = 0;
}

void SoundSource::Gen() {
    alGenSources(1, &ID);
}

void SoundSource::Play(ALuint bufferID) {
    Stop();
    alSourcei(ID, AL_BUFFER, bufferID);
    Resume();
}

void SoundSource::Play(const SoundBuffer& buffer) {
    Stop();
    alSourcei(ID, AL_BUFFER, buffer.ID);
    Resume();
}

void SoundSource::Stop() {
    alSourceStop(ID);
}

void SoundSource::Pause() {
    alSourcePause(ID);
}

void SoundSource::Resume() {
    alSourcePlay(ID);
}

void SoundSource::Loop(bool loop) {
    alSourcei(ID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

bool SoundSource::IsPlaying() {
    ALint status;
    alGetSourcei(ID, AL_SOURCE_STATE, &status);
    return status == AL_PLAYING;
}

void SoundSource::Delete() {
    if (ID)
        alDeleteSources(1, &ID);
    ID = 0;
}

void SoundSource::SetVolume(float volume) {
    alSourcef(ID, AL_GAIN, volume);
}

void SoundSource::SetPitch(float pitch) {
    alSourcef(ID, AL_PITCH, pitch);
}

void SoundSource::SetType(ALint type) {
    alSourcei(ID, AL_SOURCE_TYPE, type);
}

void SoundSource::SetRolloffFactor(float rolloffFactor) {
    alSourcef(ID, AL_ROLLOFF_FACTOR, rolloffFactor);
}

void SoundSource::SetPosition(float x, float y, float z)
{
    alSource3f(ID, AL_POSITION, x, y, z);
}

void SoundSource::SetDirection(float x, float y, float z)
{
    ALfloat Orientation[] = { x, y, z, 0.f, 1.f, 0.f };
    alSourcefv(ID, AL_ORIENTATION, Orientation);
}

template<size_t _size>
SoundSources<_size>::SoundSources() {
    for (size_t n = 0; n < _size; n++) {
        (*this)[n].Gen();
        //  (*this)[n].SetRolloffFactor(0.5f);
    }
}

template<size_t _size>
SoundSources<_size>::~SoundSources() {
    for (size_t n = 0; n < _size; n++)
        (*this)[n].Delete();
}

template<size_t _size>
SoundSource SoundSources<_size>::GetSoundSource() {
    size_t n = 0;

    while (n < (_size - 1) && this->at(n).IsPlaying())
        n++;
    return (*this)[n];
}