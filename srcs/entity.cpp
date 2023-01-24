#include "entity.h"

#define EPSILON 0.05f

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

AABB Entity::aabb() {
	return AABB(position, position + size);
}


int GetColliders(AABB area, std::vector<AABB> &colliders) {
    unsigned int x, y, z, n;
    Chunk *chunk;
    glm::ivec3 start, size;

	n = 0;
    area.min = floor(area.min);
    area.max = ceil(area.max);
    size = glm::ivec3(area.size());
    start = glm::ivec3(area.min);
     if (!(chunk = GetChunk(start.x >> 4, start.z >> 4)))
        return n;
    start.x &= 0xF;
    start.z &= 0xF;
    for (z = 0; z < size.z; z++)
        for (y = 0; y < size.y; y++)
            for (x = 0; x < size.x; x++)
                if (chunk->GetCube(start.x + x, start.y + y, start.z + z) != AIR) {
                    colliders.push_back(AABB::unit().translate(area.min + glm::vec3(x, y, z)));
					n++;
				}
    return n;
}

float MoveAxis(AABB box, float movement, std::vector<AABB>& colliders, glm::vec3 axis) {
	float depth;
    glm::vec3 d_v = axis * movement;
    float sign = glm::sign(movement);
    int index = axis.x != 0 ? 0 : (axis.y != 0 ? 1 : 2);

    AABB moved = box.translate(d_v);

    for (const auto& c : colliders) {
        if (!moved.collide(c)) {
            continue;
        }

        depth = moved.depth(c)[index];
        d_v[index] += -sign * (depth + EPSILON);
        moved = box.translate(d_v);

        if (glm::abs(d_v[index]) <= EPSILON) {
            d_v[index] = 0.0f;
            break;
        }
    }

    auto result = d_v[index];
    return glm::abs(result) <= glm::epsilon<float>() ? 0.0f : result;
}

glm::vec3 MoveBox(AABB box, glm::vec3 movement, std::vector<AABB>& colliders) {
    glm::vec3 result;
    AABB current = box;

    for (int i = 0; i < 3; i++) {
        glm::vec3 axis(0);
        axis[i] = 1.0f;

        float movement_axis = MoveAxis(box, movement[i], colliders, axis);
        current = current.translate(axis * movement_axis);
        result[i] = movement_axis;
    }

    return result;
}

void Entity::Move(glm::vec3 &movement) {
	if (hasCollision) {
		ApplyCollision(movement);
		return;
	}
	position += movement;
}

void Entity::ApplyCollision(glm::vec3& movement) {
	std::vector<AABB> colliders;

	GetColliders(this->aabb().translate(movement), colliders);
	position += MoveBox(this->aabb(), movement, colliders);

}