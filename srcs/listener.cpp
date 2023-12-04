#include "listener.h"

#define NULL 0

Listener::Listener() {
    ALCdevice* Device = alcOpenDevice(NULL);
    if (!Device)
        return;

    // Creation du contexte
    ALCcontext* Context = alcCreateContext(Device, NULL);
    if (!Context)
        return;

    // Activation du contexte
    if (!alcMakeContextCurrent(Context))
        return;

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    //  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
}

Listener::~Listener() {
    ALCcontext* Context = alcGetCurrentContext();
    ALCdevice* Device = alcGetContextsDevice(Context);

    // Desactivation du contexte
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