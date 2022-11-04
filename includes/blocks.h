#ifndef BLOCKS_CLASS_H
#define BLOCKS_CLASS_H


struct Block {
	char top;
	char side;
	char bottom;
	
	Block() {
		top = 0;
		side = 0;
		bottom = 0;
	};

	void SetTextures(char top, char side, char bottom) {
		this->top = top;
		this->side = side;
		this->bottom = bottom;
	}
};


Block blocks[256];

#endif