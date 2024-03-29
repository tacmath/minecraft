#include "camera.h"
#include "world_area.h"

// Default constructor
Camera::Camera() {
	width = 1;
	height = 1;
	far = 24.0f * RENDER_DISTANCE;
	fov = 80;
	position = glm::vec3(0.0f);
	direction = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
};

// init the camera
void Camera::Init(float windowWidth, float windowHeight, glm::vec3 pos) {
	position = pos;
	view = glm::lookAt(position, position + direction, up);
	ChangePerspective(fov, windowWidth, windowHeight, 0.1f, far);
	frustum.calculate(projection * view);
}

// treat inputs to change the camera
void Camera::Update() {
	view = glm::lookAt(position, position + direction, up);
	frustum.calculate(projection * view);
}

// treat inputs to change the camera view
void Camera::UpdateView() {
	view = glm::lookAt(position, position + direction, up);
}


// set the position of the camera with 3 float
void Camera::SetPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
}

// set the position of the camera with a vec3
void Camera::SetPosition(glm::vec3 pos) {
	position = pos;
}

//set the divection of the camera
void Camera::SetDirection(glm::vec3 direction) {
	this->direction = direction;
}

// change the prespective matrix
void Camera::ChangePerspective(float FOV, float windowWidth, float windowHeight, float near, float far) {
	if (FOV) //maybe add a SetFOV and UpdatePerspective instead
		fov = FOV;
	if (windowWidth)
		width = windowWidth;
	if (windowHeight)
		height = windowHeight;
	if (far)
		this->far = far;
	projection = glm::perspective(glm::radians(fov), (float)(width / height), near, this->far);
	frustum.calculate(projection * view);
}

float Camera::GetFOV() {
	return fov;
}

float Camera::GetScreenRatio() {
	return width / height;
}
