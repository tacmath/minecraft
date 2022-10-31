#ifndef FRUSTUM_CLASS_H
#define FRUSTUM_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include <iostream>

class Frustum
{
private:
    glm::vec3 left;
    glm::vec3 right;
    glm::vec3 origine;

public:
	Frustum(){};
    ~Frustum(){};

    void setOrigine(glm::vec3 origine) {
        this->origine = origine;
  //          std::cout << "origine: " << origine.x << " " << origine.y << " " << origine.z << std::endl;
    }

    void calculate(glm::vec3 direction, float fov) {
        fov = 90;
        left = glm::rotate(direction, glm::radians(fov / 2), glm::vec3(0, 1, 0));
        right = glm::rotate(direction, glm::radians(-fov / 2), glm::vec3(0, 1, 0));
   //     std::cout << "left (" << origine.x << " " << origine.z << ") left (" << left.x << " " << left.z << ") right (" << right.x << " " << right.z << ")" << std::endl;
    }

    inline bool isVisible(float x, float z, float size) {


        return (true);
    }

};

#endif