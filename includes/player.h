#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"
#include "raycast.h"
#include "camera.h"
#include <functional>

#define PLAYER_RANGE 5

class Player : public Entity {
private:
	// the updateCallback
	std::function<void(Player&)> updateCallback;

	glm::vec3 cameraOffset;
public:
	RayCastInfo selectedCube;
	Camera		camera;
	char		selectedItem; //later use an inventory and an index to the selected slot

	Player() {
		look = glm::vec3(0, 0, -1);
		size = glm::vec3(0, 2, 0);
		position = glm::vec3(0, 60, 0);
		cameraOffset = glm::vec3(0);
		hasCollision = false;
		selectedItem = 4;
		updateCallback = [](Player) {};
	}

	void Init(glm::vec2 windowSize) {
		camera.Init(windowSize.x, windowSize.y, position + cameraOffset);
		camera.SetDirection(look);
		camera.Update();
	}

	void Update() {
		selectedCube = rayCastGetCube(position, look, PLAYER_RANGE);
		camera.SetPosition(position + cameraOffset);
		camera.SetDirection(look);
		camera.Update();
		updateCallback(*this);
	}

	void SetUpdateCallback(std::function<void(Player&)> callback) {
		updateCallback = callback;
	}

	void UpdateCallback() {
		updateCallback(*this);
	}
};

#endif