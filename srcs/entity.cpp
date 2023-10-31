#include "entity.h"

#define EPSILON 0.05f

Entity::Entity() {
	ID = 0;
    lastStep = 0;
    hasCollision = true;
    isGrounded = true;
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

bool Entity::Grounded() {
    return isGrounded;
}


int GetColliders(AABB area, std::vector<AABB> &colliders) { // changer pour qu'il soit plus opti et perenne un  std array
    int x, y, z, n;
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

void OccludeColliders(AABB area, std::vector<AABB> &colliders) {
    for (unsigned int n = 0; n < colliders.size(); n++) {
        if (area.collide(colliders[n])) {
            colliders.erase(colliders.begin() + n);
            n--;
        }
    }
}

float MoveAxis(AABB box, float movement, std::vector<AABB>& colliders, glm::vec3 axis) {
	float depth;
    glm::vec3 d_v = axis * movement;
    float sign = movement < 0.0f ? -1.0f : 1.0f;
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

    float result = d_v[index];
    return glm::abs(result) <= glm::epsilon<float>() ? 0.0f : result;
}

glm::vec3 MoveBox(AABB box, glm::vec3 movement, std::vector<AABB>& colliders) {
    glm::vec3 result;
    char moveOrder[3] = {1, 0, 2}; // y x z
    AABB current = box;
    
    if (movement.x > movement.z) {
        moveOrder[1] = 2;
        moveOrder[2] = 0;
    }

    for (int n = 0; n < 3; n++) {
        int axisId = moveOrder[n];
        glm::vec3 axis(0);
        axis[axisId] = 1.0f;

        float movement_axis = MoveAxis(current, movement[axisId], colliders, axis);
        current = current.translate(axis * movement_axis);
        result[axisId] = movement_axis;
    }

    return result;
}

// TODO not call playStepSound if entity is too far away
static inline void playStepSound(glm::vec3 soundPos) {
    // TODO if no solid block is found get a block by using GetColliders(this->aabb().translate(glm::vec3(0, -0.1f, 0)), colliders)
    unsigned char cubeID = GetCubeAt((int)floor(soundPos.x), (int)floor(soundPos.y - 0.1f), (int)floor(soundPos.z));
    Chunk::blocks[cubeID].PlayStepSound(soundPos.x, soundPos.y, soundPos.z);
}

void Entity::Move(glm::vec3 &movement) {
	if (hasCollision) {
        float nbStep = glm::length(movement) / 0.4f;
        if (nbStep < 1.0f)
		    ApplyCollision(movement);
        else {
            glm::vec3 step = movement / nbStep;
            for (int n = 0; n < nbStep; n++)
                ApplyCollision(step);
        }
        if (isGrounded && lastStep >= 2.0f) { // for later some entity may have unique step sound and others mais not have any
            lastStep = 0.0f;
            playStepSound(glm::vec3(position.x + size.x * 0.5f, position.y, position.z + size.z * 0.5f));
        }
		return;
	}
	position += movement;
}

void Entity::ApplyCollision(glm::vec3& movement) {
	static std::vector<AABB> colliders;
    AABB entityAABB;

    colliders.clear();
    entityAABB = this->aabb();
	if (GetColliders(entityAABB.translate(movement), colliders) >= 0) {
        float oldVerticalMove = movement.y;
        OccludeColliders(entityAABB, colliders);
        movement = MoveBox(entityAABB, movement, colliders);
        isGrounded = (oldVerticalMove < 0 && oldVerticalMove != movement.y) ? true : false;
        lastStep += glm::length(glm::vec2(movement.x, movement.z));
    }
	position += movement;
}