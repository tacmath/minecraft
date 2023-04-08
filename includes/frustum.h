#ifndef FRUSTUM_CLASS_H
#define FRUSTUM_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include <iostream>

#define ABS(x) (((x) < 0) ? -(x) : (x))

class Frustum
{
private:
    glm::vec4 left;
    glm::vec4 right;
    glm::vec4 top;
    glm::vec4 bottom;
    glm::vec4 near;
    glm::vec4 far;

public:
	Frustum(){};
    ~Frustum(){};

    void calculate(const glm::mat4 &mat) {
        for (int i = 4; i--; ) {
            left[i]   = mat[i][3] + mat[i][0];
            right[i]  = mat[i][3] - mat[i][0];
            bottom[i] = mat[i][3] + mat[i][1];
            top[i]    = mat[i][3] - mat[i][1];
            near[i]   = mat[i][3] + mat[i][2];
            far[i]    = mat[i][3] - mat[i][2];
        }
    }

    inline bool isOnOrForwardPlan(const glm::vec4 &plan, const glm::vec3 &point, int radius) const
    {
        const float r = radius * (ABS(plan.x) + ABS(plan.z)) + 128 * ABS(plan.y);
        return plan.x * point.x + plan.y * point.y + plan.z * point.z + plan.w >= -r;
    }

    inline bool chunkIsVisible(int x, int z, int radius = 8) const {
        glm::vec3 pos((x << 4) + 8, 128, (z << 4) + 8);

        if (isOnOrForwardPlan(right, pos, radius) &&
            isOnOrForwardPlan(left, pos, radius) &&
            isOnOrForwardPlan(top, pos, radius) &&
            isOnOrForwardPlan(bottom, pos, radius))
            return (true);
        return (false);
    }

};

#endif