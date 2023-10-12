#include "soundSource.h"

inline SoundSource::SoundSource() {
    ID = 0;
}

inline void SoundSource::Gen() {
    alGenSources(1, &ID);
}

inline void SoundSource::Play(ALuint bufferID) {
    Stop();
    alSourcei(ID, AL_BUFFER, bufferID);
    Resume();
}

inline void SoundSource::Play(const SoundBuffer& buffer) {
    Stop();
    alSourcei(ID, AL_BUFFER, buffer.ID);
    Resume();
}

inline void SoundSource::Stop() {
    alSourceStop(ID);
}

inline void SoundSource::Pause() {
    alSourcePause(ID);
}

inline void SoundSource::Resume() {
    alSourcePlay(ID);
}

inline void SoundSource::Loop(bool loop) {
    alSourcei(ID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

inline bool SoundSource::IsPlaying() {
    ALint status;
    alGetSourcei(ID, AL_SOURCE_STATE, &status);
    return status == AL_PLAYING;
}

inline void SoundSource::Delete() {
    if (ID)
        alDeleteSources(1, &ID);
    ID = 0;
}

inline void SoundSource::SetVolume(float volume) {
    alSourcef(ID, AL_GAIN, volume);
}

inline void SoundSource::SetPitch(float pitch) {
    alSourcef(ID, AL_PITCH, pitch);
}

inline void SoundSource::SetType(ALint type) {
    alSourcei(ID, AL_SOURCE_TYPE, type);
}

inline void SoundSource::SetRolloffFactor(float rolloffFactor) {
    alSourcef(ID, AL_ROLLOFF_FACTOR, rolloffFactor);
}

inline void SoundSource::SetPosition(float x, float y, float z)
{
    alSource3f(ID, AL_POSITION, x, y, z);
}

inline void SoundSource::SetDirection(float x, float y, float z)
{
    ALfloat Orientation[] = { x, y, z, 0.f, 1.f, 0.f };
    alSourcefv(ID, AL_ORIENTATION, Orientation);
}

template<size_t _size>
inline SoundSources<_size>::SoundSources() {
    for (size_t n = 0; n < _size; n++) {
        (*this)[n].Gen();
        //  (*this)[n].SetRolloffFactor(0.5f);
    }
}

template<size_t _size>
inline SoundSources<_size>::~SoundSources() {
    for (size_t n = 0; n < _size; n++)
        (*this)[n].Delete();
}

template<size_t _size>
inline SoundSource SoundSources<_size>::GetSoundSource() {
    size_t n = 0;

    while (n < (_size - 1) && this->at(n).IsPlaying())
        n++;
    return (*this)[n];
}