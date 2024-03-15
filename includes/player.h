#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "entity.h"
#include "raycast.h"
#include "camera.h"
#include "listener.h"
#include <array>
#include <functional>

#define INVENTORY_SIZE 9

typedef std::array<char, INVENTORY_SIZE> inventory_t;

//maybe add a playerInfo struct and add a GetInfo() method

class Player : public Entity {
	#define PLAYER_RANGE 5
private:
	// the updateCallback
	std::function<void(const Player&)> updateCallback;
	std::function<void(const Player&)> inventoryUpdateCallback;
	glm::vec3 cameraOffset;
	unsigned	selectedSlot; //later use an inventory and an index to the selected slot
	inventory_t inventory;

public:
	RayCastInfo selectedCube;
	Camera		camera;


	Player() {
		look = glm::vec3(0, 0, -1);
		size = glm::vec3(0.4f, 1.65f, 0.4f);
		position = glm::vec3(0, 60, 0);
		velocity = glm::vec3(0);
		cameraOffset = glm::vec3(0.2f, 1.5f, 0.2f);
		hasCollision = false;
		selectedSlot = 0;
		inventory.fill(0);
		inventory[0] = 1;
		inventory[1] = 2;
		inventory[2] = 3;
		inventory[3] = 4;
		updateCallback = [](const Player&) {};
		inventoryUpdateCallback = [](const Player&) {};
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

	int SelectedItem() {
		return inventory[selectedSlot];
	}

	void SetSelectedSlot(unsigned selectedSlot) {
		this->selectedSlot = selectedSlot;
		inventoryUpdateCallback(*this);
	}

	unsigned GetSelectedSlot() const {
		return selectedSlot;
	}

	void SetInventoryItem(unsigned selectedSlot, char itemId) {
		inventory[selectedSlot] = itemId;
		inventoryUpdateCallback(*this);
	}

	const inventory_t& GetInventory() const {
		return inventory;
	}

	void SetUpdateCallback(std::function<void(const Player&)> callback) {
		updateCallback = callback;
	}

	void SetInventoryUpdateCallback(std::function<void(const Player&)> callback) {
		inventoryUpdateCallback = callback;
	}

	void UpdateCallback() {
		updateCallback(*this);
	}

	void InventoryUpdateCallback() {
		inventoryUpdateCallback(*this);
	}
};

#endif