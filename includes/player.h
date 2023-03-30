#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"
#include "raycast.h"
#include "camera.h"
#include <functional>

#define PLAYER_RANGE 5

//maybe add a playerInfo struct and add a GetInfo() method

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
		size = glm::vec3(0.4f, 1.65f, 0.4f);
		position = glm::vec3(0, 60, 0);
		cameraOffset = glm::vec3(0.2f, 1.5f, 0.2f);
		hasCollision = false;
		selectedItem = 4;
		updateCallback = [](Player&) {};
	}

	void Init(glm::vec2 windowSize) {
		camera.Init(windowSize.x, windowSize.y, position + cameraOffset);
		camera.SetDirection(look);
		camera.Update();
	}

	void UpdateRayCast() {
		selectedCube = rayCastGetCube(position + cameraOffset, look, PLAYER_RANGE);
	}

	void Update() {
		glm::vec3 eyePos;

		eyePos = position + cameraOffset;
		selectedCube = rayCastGetCube(eyePos, look, PLAYER_RANGE);
		camera.SetPosition(eyePos);
		camera.SetDirection(look);
		camera.Update();

		Listener::SetPosition(eyePos.x, eyePos.y, eyePos.z);
		Listener::SetDirection(look.x, look.y, look.z);
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