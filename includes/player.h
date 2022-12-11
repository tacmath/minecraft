#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"
#include "raycast.h"

#define PLAYER_RANGE 5

class Player : public Entity {
public:
	RayCastInfo selectedCube;

	Player() {
		look = glm::vec3(0, 0, -1);
		size = glm::vec3(0, 2, 0);
		position = glm::vec3(0, 60, 0);
		hasCollision = false;
	}

};

#endif