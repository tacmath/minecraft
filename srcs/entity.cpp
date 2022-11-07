#include "entity.h"

Entity::Entity() {
	ID = 0;
	position = glm::vec3(0);
	size = glm::vec3(0);
	look = glm::vec3(0);
}

void Entity::SetPosition(glm::vec3 pos) {
	this->position = pos;
}
void Entity::SetPosition(float x, float y, float z) {
	this->position = glm::vec3(x, y, z);
}
void Entity::SetSize(glm::vec3 size) {
	this->size = size;
}
void Entity::SetSize(float x, float y, float z) {
	this->size = glm::vec3(x, y, z);
}
void Entity::SetLook(glm::vec3 look) {
	this->look = look;
}
void Entity::SetLook(float x, float y, float z) {
	this->look = glm::vec3(x, y, z);
}