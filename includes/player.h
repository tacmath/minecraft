#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"

class Player : public Entity {
public:
	Player() {
		look = glm::vec3(0, 0, -1);
		size = glm::vec3(0, 2, 0);
		position = glm::vec3(0, 60, 0);
		hasCollision = false;
	}

};

#endif