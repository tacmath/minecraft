#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glm/glm.hpp>
#include "frustum.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#define NORMAL_PERSPECTIVE 0
#define UP_PERSPECTIVE 1

class Camera
{
private:
	float width;
	float height;
	float fov;

	//position of the camera
	glm::vec3 position;

	//direction where the camera is looking
	glm::vec3 direction;

	glm::vec3 up;

public:
	// the frustum of the camera
	// https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
	Frustum frustum;

	// the view matrix
	glm::mat4 view;

	// the projection matrix
	glm::mat4 projection;



	// Default constructor
	Camera();

	// init the camera
	void Init(float windowWidth, float windowHeight, glm::vec3 pos);

	// set the position of the camera with 3 float
	void SetPosition(float x, float y, float z);

	// set the position of the camera with a vec3
	void SetPosition(glm::vec3 pos);

	//set the divection of the camera
	void SetDirection(glm::vec3 direction);

	// change the prespective matrix
	void ChangePerspective(float FOV, float windowWidth, float windowHeight, float near, float far);

	// update the camera matrice using its data
	void Update();

	// treat inputs to change the camera view
	void UpdateView();
};

#endif