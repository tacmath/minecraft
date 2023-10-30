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
	// maybe set as static to be called as Sound::sources.GetSoundSource()
	// and maybe make SoundSources dynamic with vector
	SoundSources<MAX_SOUND_SOURCES> sources;

	~Sound() {
		sources.Delete();
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