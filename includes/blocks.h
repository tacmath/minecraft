#ifndef BLOCKS_CLASS_H
#define BLOCKS_CLASS_H

#include "soundBuffer.h"
#include "soundSource.h"

#define MAX_BLOCK_NB 255

struct Block {
	uint8_t			top;
	uint8_t			side;
	uint8_t			bottom;
	SoundBuffers	soundBuffers; // a la place besoin de stocker l'id du buffer ou l'id de la ou est le buffer pour �viter plusieurs m�me buffer ou un pointeur
	const SoundSources<MAX_SOUND_SOURCES> *soundSources; //maybe use a shared_ptr or shared_ptr and weak_ptr or set it as static in Sound class
	
	Block() {
		Clean();
	};

	void SetTextures(uint8_t top, uint8_t side, uint8_t bottom) {
		this->top = top;
		this->side = side;
		this->bottom = bottom;
	}

	void SetTexture(std::string& type, uint8_t textureIndex) {
		if (type == "Top")
			this->top = textureIndex;
		else if (type == "Side")
			this->side = textureIndex;
		else if (type == "Bottom")
			this->bottom = textureIndex;
	}

	void PlaySound(float x, float y, float z) {
		ALuint buffer = soundBuffers.GetRandom();

		if (buffer != -1) {
			SoundSource source = soundSources->GetSoundSource();
			source.SetPosition(x, y, z);
			source.Play(buffer);
		}
	}

	void Clean() {
		top = 0;
		side = 0;
		bottom = 0;
		soundSources = 0;
	}

	void Delete() {

	}
};

#endif