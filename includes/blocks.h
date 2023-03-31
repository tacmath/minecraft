#ifndef BLOCKS_CLASS_H
#define BLOCKS_CLASS_H

#include "sound.h"

struct Block {
	char top;
	char side;
	char bottom;
	SoundBuffer soundBuffer;
	SoundSource sound;
	
	Block() {
		top = 0;
		side = 0;
		bottom = 0;
	};

	void Delete() {
		sound.Delete();
		soundBuffer.Delete();
	};

	void SetTextures(char top, char side, char bottom) {
		this->top = top;
		this->side = side;
		this->bottom = bottom;
	}
	void SetTexture(std::string& type, char textureIndex) {
		if (type == "Top")
			this->top = textureIndex;
		else if (type == "Side")
			this->side = textureIndex;
		else if (type == "Bottom")
			this->bottom = textureIndex;
	}

	void PlayBreakSound(float x, float y, float z) {
		if (soundBuffer.ID) {
			sound.SetPitch(1.0f);
			sound.SetPosition(x, y, z);
			sound.Play(soundBuffer);
		}
	}

	void PlayPlaceSound(float x, float y, float z) {
		if (soundBuffer.ID) {
			sound.SetPitch(0.6f);
			sound.SetPosition(x, y, z);
			sound.Play(soundBuffer);
		}
	}

};

#endif