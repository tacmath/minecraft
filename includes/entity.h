#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>

unsigned char GetCubeAt(int x, int y, int z);

class Entity {		//maybe add the chunk the entity is in and update the chunk when x >> 4 != chunk.posx && z >> 4 != chunk.posz
private:
	float force = 0.0f;
	float gravity = -9.81f;
	bool isOnTheGround = false;
public:
	glm::vec3 size;
	glm::vec3 look;
	glm::vec3 position;
	bool hasCollision;

	int ID;

	Entity();

	void Move(glm::vec3 &movement, float frequence) {
		movement.y += updateFromGravity(frequence);
		isOnTheGround = true;
		if (!CheckCollisionDown(movement.y)) {
				isOnTheGround = false;
				position.y += movement.y;
		}
		else {
			position.y = ceil(position.y);
		}
			
		if (!CheckCollisionXZ(movement.x, movement.z)) {
			position.x += movement.x;
			position.z += movement.z;
		}
	}

	float updateFromGravity(float frequence) {
		force += gravity * frequence;
		if (isOnTheGround == true) force = 0;
		else if (force > 80) force = 80;
		return force / 5000;
	}

	void Jump() {
		if (isOnTheGround == false) return ;
		isOnTheGround = false;
		force = 80;
	}

	void SetPosition(glm::vec3 pos);
	void SetPosition(float x, float y, float z);
	void SetSize(glm::vec3 size);
	void SetSize(float x, float y, float z);
	void SetLook(glm::vec3 look);
	void SetLook(float x, float y, float z);

private:
	void CheckCollision(glm::vec3& movement) {
		position.y += movement.y;
		if (movement.y < 0 && GetCubeAt((int)floor(position.x), (int)floor(position.y - size.y), (int)floor(position.z)))
			position.y = floor(position.y + 1);
		position.x += movement.x;
		if (GetCubeAt((int)floor(position.x), (int)floor(position.y - size.y), (int)floor(position.z)))
			position.x = floor(position.x + (movement.x < 0)) - 0.001f * (movement.x >= 0);
		position.z += movement.z;
		if (GetCubeAt((int)floor(position.x), (int)floor(position.y - size.y), (int)floor(position.z)))
			position.z = floor(position.z + (movement.z < 0)) - 0.001f * (movement.z >= 0);
	}

	bool CheckCollisionDown(float moveY) {
		if (moveY >= 0) return (0);
		int intMoveY = (int)floor(moveY);
		for (int i = 0; i > intMoveY; i--)
			if (GetCubeAt((int)floor(position.x), (int)floor(position.y - size.y + i), (int)floor(position.z))) return (1);
		return (0);
	}

	bool CheckCollisionXZ(float moveX, float moveZ) {

		if (moveX == 0 && moveZ == 0) return (0);

		return (0);
	}
};

#endif