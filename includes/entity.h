#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include "raycast.h"
#include "aabb.h"

unsigned char GetCubeAt(int x, int y, int z);
int GetColliders(AABB area, std::vector<AABB>& colliders);

#define AIR_FRICTION  0.1f
#define GRAVITY       40.0f//9.1f
#define JUMP_VELOCITY 12.0f

class Entity {		//maybe add the chunk the entity is in and update the chunk when x >> 4 != chunk.posx && z >> 4 != chunk.posz
private:
	float		lastStep;
	bool		isGrounded;

public:
	glm::vec3	size;
	glm::vec3	look;
	glm::vec3	position;
	glm::vec3	velocity; // in meter per second
	bool		hasCollision;
	int			ID;

	Entity();

	void Move(const glm::vec3& velocity, float latency);
	void Jump();

	void SetPosition(glm::vec3 pos);
	void SetPosition(float x, float y, float z);
	void SetSize(glm::vec3 size);
	void SetSize(float x, float y, float z);
	void SetLook(glm::vec3 look);
	void SetLook(float x, float y, float z);
	bool Grounded();
	AABB aabb();

private:
	void ApplyCollision(glm::vec3& movement);
	glm::vec3 ComputeMovement(float latency);
};

#endif