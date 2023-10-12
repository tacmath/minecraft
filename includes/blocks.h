#ifndef BLOCKS_CLASS_H
#define BLOCKS_CLASS_H

#include "sound.h"

#define MAX_BLOCK_NB 255

struct Block {
	uint8_t top;
	uint8_t side;
	uint8_t bottom;
//	SoundBuffer soundBuffer; // a la place besoin de stocker l'id du buffer ou l'id de la ou est le buffer pour �viter plusieurs m�me buffer
	
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
	//	if (soundBuffer.ID) {
	//		sound.SetPosition(x, y, z);
	//		sound.Play(soundBuffer);
	//	}
	}

	void Clean() {
		top = 0;
		side = 0;
		bottom = 0;
	}

	void Delete() {

	}
};

#endif