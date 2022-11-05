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

    /*
        Left:0.75 0 -1 0
        right:-0.75 0 -1 0
        bottom:0 1 -1 -60
        top:0 -1 -1 60
        near:0 0 -2.0002 -0.20002
        far:0 0 0.000199914 0.20002
    */

    void calculate(glm::mat4 mat) {
        for (int i = 4; i--; ) {
            left[i]   = mat[i][3] + mat[i][0];
            right[i]  = mat[i][3] - mat[i][0];
            bottom[i] = mat[i][3] + mat[i][1];
            top[i]    = mat[i][3] - mat[i][1];
            near[i]   = mat[i][3] + mat[i][2];
            far[i]    = mat[i][3] - mat[i][2];
        }
        
        std::cout << "Left:" << left[0] << " " << left[1] << " " << left[2] << " " << left[3] << std::endl;
        std::cout << "right:" << right[0] << " " << right[1] << " " << right[2] << " " << right[3] << std::endl;
    /*    std::cout << "bottom:" << bottom[0] << " " << bottom[1] << " " << bottom[2] << " " << bottom[3] << std::endl;
        std::cout << "top:" << top[0] << " " << top[1] << " " << top[2] << " " << top[3] << std::endl;
        std::cout << "near:" << near[0] << " " << near[1] << " " << near[2] << " " << near[3] << std::endl;
        std::cout << "far:" << far[0] << " " << far[1] << " " << far[2] << " " << far[3] << std::endl; */
        
    }

    inline bool isVisible(float x, float z, float size) {

        return (true);
    }

};

#endif