#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include "raycast.h"

unsigned char GetCubeAt(int x, int y, int z);

class Entity {		//maybe add the chunk the entity is in and update the chunk when x >> 4 != chunk.posx && z >> 4 != chunk.posz
public:
	glm::vec3 size;
	glm::vec3 look;
	glm::vec3 position;
	bool hasCollision;
	int ID;

	Entity();

	void Move(glm::vec3 &movement) {
		if (hasCollision) {
			ApplyCollision(movement);
			return;
		}
		position += movement;
	}

	void SetPosition(glm::vec3 pos);
	void SetPosition(float x, float y, float z);
	void SetSize(glm::vec3 size);
	void SetSize(float x, float y, float z);
	void SetLook(glm::vec3 look);
	void SetLook(float x, float y, float z);

private:
	void ApplyCollision(glm::vec3& movement) {
		/*glm::vec3 origin = position + glm::vec3(0, -size.y, 0);
		glm::vec3 norm = glm::normalize(movement);
		float velocity = length(movement);
		RayCastInfo info;

		position += movement;
		info = rayCastGetCube(origin, glm::vec3(0, movement.y, 0), abs(velocity * norm.y));
		if (info.id != AIR && info.range <= abs(velocity * norm.y))
			position.y = info.hit.y + size.y;
		info = rayCastGetCube(origin, glm::vec3(0, 0, movement.z), abs(velocity * norm.z));
		if (info.id != AIR && info.range <= abs(velocity * norm.z))
			position.z = info.hit.z;
		info = rayCastGetCube(origin, glm::vec3(movement.x, 0, 0), abs(velocity * norm.x));
		if (info.id != AIR && info.range <= abs(velocity * norm.x))
			position.x = info.hit.x;*/

		
		
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
};

#endif