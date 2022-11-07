#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

class Entity {
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 look;
public:
	int ID;
	Entity();

	void SetPosition(glm::vec3 pos);
	void SetPosition(float x, float y, float z);
	void SetSize(glm::vec3 size);
	void SetSize(float x, float y, float z);
	void SetLook(glm::vec3 look);
	void SetLook(float x, float y, float z);
};

#endif