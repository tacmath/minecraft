#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"

struct RayCastInfo {
	unsigned char id;
	glm::ivec3 position;
	glm::ivec3 side;
	float		range;

	RayCastInfo() {
		id = 0;
		position = glm::ivec3(0);
		range = 0;
	}
};

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