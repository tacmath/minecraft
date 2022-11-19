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
		if (!CheckCollision(movement)) {
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
	/*
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
	*/

	bool CheckCollisionDown(float moveY) {
		if (moveY >= 0) return (0);
		int intMoveY = (int)floor(moveY);
		for (int i = 0; i > intMoveY; i--)
			if (GetCubeAt((int)floor(position.x), (int)floor(position.y - size.y + i), (int)floor(position.z))) return (1);
		return (0);
	}

	void printVect(glm::vec3 vect) {
		std::cout << "x: " << vect.x << " y : " << vect.y << " z : " << vect.z << std::endl;
	}

	/// CALCULATE COLLISION

	glm::vec3 _calculateStep(glm::vec3 movement, float minStepPrecision) {
		glm::vec3 step = glm::abs(movement);
		while (step.x > minStepPrecision || step.y > minStepPrecision || step.z > minStepPrecision)
			step = glm::vec3(step.x / 2, step.y / 2, step.z / 2);
		return (step);
	}

	bool CheckCollision(glm::vec3 movement) {
		glm::vec3 step;					// correspond au pas du raycasting
		glm::uvec3 sens;				// correspond au sens de direction -1 ou 1
		glm::vec3 newPosition;
		glm::vec3 intNewPosition;
		glm::vec3 collideSize;


		step = _calculateStep(movement, 0.5f);
		sens = glm::uvec3(movement.x > 0 ? 1 : -1, movement.y > 0 ? 1 : -1, movement.z > 0 ? 1 : -1);
		printVect(step);

		float istep = 0.0f;
		float igoal = 0;
		if (step.x > step.y && step.x > step.z) {
			istep = step.x;
			igoal = glm::abs(movement.x);
		}
		else if (step.y > step.x && step.y > step.z) {
			istep = step.y;
			igoal = glm::abs(movement.y);
		}
		else {
			istep = step.z;
			igoal = glm::abs(movement.z);
		}

		step = glm::vec3(step.x * sens.x, step.y * sens.y, step.z * sens.z);
		
		
		//collideSize = glm::vec3(step.x > 0 ? size.x / 2 : -size.x / 2, size.y, step.z > 0 ? size.z / 2 : -size.z);
		/*for (int i = 1; istep * i < igoal; i++) {
			newPosition = glm::vec3(position.x + collideSize.x + (step.x * i), position.y + collideSize.y + (step.y * i), position.z + collideSize.z + (step.z * i));
			intNewPosition = glm::vec3(step.x > 0 ? glm::floor(newPosition.x) : glm::ceil(newPosition.x), step.y > 0 ? glm::floor(newPosition.y) : glm::ceil(newPosition.y), step.z > 0 ? glm::floor(newPosition.z) : glm::ceil(newPosition.z));
			if (GetCubeAt(intNewPosition.x, intNewPosition.y, intNewPosition.z)) {
				return (true);
			}
		}*/
		return (false);
	}
};

#endif