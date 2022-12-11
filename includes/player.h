#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"
#include "raycast.h"
#include "camera.h"

#define PLAYER_RANGE 5

class Player : public Entity {
private:
	glm::vec3 cameraOffset;
public:
	RayCastInfo selectedCube;
	Camera		camera;

	Player() {
		look = glm::vec3(0, 0, -1);
		size = glm::vec3(0, 1.7f, 0);
		position = glm::vec3(0, 60, 0);
		cameraOffset = glm::vec3(0);
		hasCollision = false;
	}

	void Init(glm::vec2 windowSize) {
		camera.Init(windowSize.x, windowSize.y, position + cameraOffset);
		camera.SetDirection(look);
		camera.Update();
	}

	void Update() {
		camera.SetPosition(position + cameraOffset);
		camera.SetDirection(look);
		camera.Update();
	}
};

#endif