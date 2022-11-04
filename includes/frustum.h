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
    }

    inline bool isVisible(float x, float z, float size) {
        
        return (true);
    }

};

#endif