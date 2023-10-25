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
	static SoundSources<MAX_SOUND_SOURCES> sources;

	~Sound() {
		DeleteSounds();
	}

	void AddSounds(const std::vector<std::string>& soundFiles) {
		for (std::string soundFile : soundFiles)
			sounds.Add(soundFile.c_str());
	}

	void AddSounds(const SoundBuffers& soundBuffers) {
		for (SoundBuffer sound : soundBuffers)
			sounds.Add(sound);
	}

	void DeleteSounds() {
		sounds.Clear();
	}
};