#ifndef BLOCKS_CLASS_H
#define BLOCKS_CLASS_H

#include "sound.h"

struct Block {
	char top;
	char side;
	char bottom;
	SoundBuffer soundBuffer; // a la place besoin de stocker l'id du buffer ou l'id de la ou est le buffer pour �viter plusieurs m�me buffer
	SoundSource sound;
	
	Block() {
		Clean();
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

	void PlaySound(float x, float y, float z) {
		if (soundBuffer.ID) {
			sound.SetPosition(x, y, z);
			sound.Play(soundBuffer);
		}
	}

	void Clean() {
		top = 0;
		side = 0;
		bottom = 0;
		soundBuffer.ID = 0;
		sound.ID = 0;
	}
};

#endif