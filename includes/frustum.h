#ifndef FRUSTUM_CLASS_H
#define FRUSTUM_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include <iostream>

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

    void calculate(glm::mat4 mat) {
        for (int i = 4; i--; ) {
            left[i]   = mat[i][3] + mat[i][0];
            right[i]  = mat[i][3] - mat[i][0];
            bottom[i] = mat[i][3] + mat[i][1];
            top[i]    = mat[i][3] - mat[i][1];
            near[i]   = mat[i][3] + mat[i][2];
            far[i]    = mat[i][3] - mat[i][2];
        }
        
        /*std::cout << "Left:" << left[0] << " " << left[1] << " " << left[2] << " " << left[3] << std::endl;
        std::cout << "right:" << right[0] << " " << right[1] << " " << right[2] << " " << right[3] << std::endl;*/
    /*    std::cout << "bottom:" << bottom[0] << " " << bottom[1] << " " << bottom[2] << " " << bottom[3] << std::endl;
        std::cout << "top:" << top[0] << " " << top[1] << " " << top[2] << " " << top[3] << std::endl;
        std::cout << "near:" << near[0] << " " << near[1] << " " << near[2] << " " << near[3] << std::endl;
        std::cout << "far:" << far[0] << " " << far[1] << " " << far[2] << " " << far[3] << std::endl; */
        
    }

    inline bool isOnOrForwardPlan(glm::vec4 &plan, glm::vec3 &point) const
    {
        const float r = 8 * (std::abs(plan.x) + std::abs(plan.z)) +
            128 * std::abs(plan.y);
        return glm::dot(glm::vec3(plan), point) - -plan.w >= -r;
    }

    inline bool chunkIsVisible(int x, int z) {
        glm::vec3 pos((x << 4) + 8, 128, (z << 4) + 8);

        if (isOnOrForwardPlan(right, pos) &&
            isOnOrForwardPlan(left, pos) &&
            isOnOrForwardPlan(top, pos) &&
            isOnOrForwardPlan(bottom, pos))
            return (true);
        return (false);
    }

};

#endif