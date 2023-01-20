#ifndef AABB_CLASS_H
#define AABB_CLASS_H

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

// maybe use a template for different types of aabb and use a typedef like typedef _AABB<int> AABBi;

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	AABB() = default;

	AABB(const glm::vec3 min, const glm::vec3 max) {
		this->min = min;
		this->max = max;
	}

	void set(const glm::vec3 &min, const glm::vec3 &max) {
		this->min = min;
		this->max = max;
	}

    AABB translate(glm::vec3 &v) const {
        return AABB(min + v, max + v);
    }

    AABB scale(glm::vec3& v) const {
        return AABB(min, size() * v);
    }

    AABB scale(float value) const {
        return AABB(min, size() * value);
    }

    glm::vec3 center() const {
        return (min + max) * 0.5f;
    }

    glm::vec3 size() const {
        return max - min;
    }

    bool collide(const AABB &other) const {
        return
            this->min.x <= other.max.x && this->max.x >= other.min.x &&
            this->min.y <= other.max.y && this->max.y >= other.min.y &&
            this->min.z <= other.max.z && this->max.z >= other.min.z;
    }

    bool contain(const glm::vec3 &point) const {
        return
            point.x >= this->min.x && point.x <= this->max.x &&
            point.y >= this->min.y && point.y <= this->max.y &&
            point.z >= this->min.z && point.z <= this->max.z;
    }

    std::string to_string() const {
        return "AABB(" + glm::to_string(min) + ", " + glm::to_string(max) + ")";
    }
};

#endif
