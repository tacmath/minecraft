#pragma once

#include "soundBuffer.h"
#include "soundSource.h"
#include "listener.h"
#include "music.h"

class Sound {
private:
	Listener		listener;
	Music			music;
	SoundBuffers	sounds;

public:
	void AddSounds(std::vector<std::string> soundFiles) {

	}

	void DeleteSounds() {
		sounds.Clear();
	}
};