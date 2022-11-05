#include "camera.h"

// Default constructor
Camera::Camera() {
	perspective = NORMAL_PERSPECTIVE;
	firstClick = true;
	oldMousePos = glm::vec2(0.0f);
	width = 0;
	height = 0;
	fov = 90;
	posision = glm::vec3(0.0f);
	direction = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);
};

// init the camera
void Camera::Init(float windowWidth, float windowHeight, glm::vec3 pos) {
	width = windowWidth;
	height = windowHeight;
	posision = pos;
	view = glm::lookAt(posision, posision + direction, up);
	projection = glm::perspective(glm::radians(fov), (float)(windowWidth / windowHeight), 0.1f, 1000.0f);
	frustum.calculate(projection * view);
}

// treat inputs to change the camera
void Camera::Inputs(GLFWwindow* window) {
	this->KeyInputs(window);
	this->MouseInputs(window);
	view = glm::lookAt(posision, posision + direction, up);
	frustum.calculate(projection * view);
	if (perspective == UP_PERSPECTIVE)
		view = glm::lookAt(posision + glm::vec3(-10, 200, 0), posision, up);
}

// set the position of the camera with 3 float
void Camera::SetPosition(float x, float y, float z) {
	posision.x = x;
	posision.y = y;
	posision.z = z;
	view = glm::lookAt(posision, posision + direction, up);
}

// set the position of the camera with a vec3
void Camera::SetPosition(glm::vec3 pos) {
	posision = pos;
	view = glm::lookAt(posision, posision + direction, up);
}

//set the divection of the camera
void Camera::SetDirection(glm::vec3 direction) {
	this->direction = direction;
	view = glm::lookAt(posision, posision + direction, up);
}


// change the prespective matrix
void Camera::ChangePerspective(float FOV, float windowWidth, float windowHeight, float near, float far) {
	fov = FOV;
	projection = glm::perspective(glm::radians(FOV), (float)(windowWidth / windowHeight), near, far);
	frustum.calculate(projection);
}

// treat key inputs to change the camera position
void Camera::KeyInputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		posision += speed * direction;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		posision += speed * -glm::normalize(glm::cross(direction, up));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		posision += speed * -direction;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		posision += speed * glm::normalize(glm::cross(direction, up));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		posision += speed * up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		posision += speed * -up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 10.0f;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		speed = 0.4f;

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		perspective = UP_PERSPECTIVE;
	else
		perspective = NORMAL_PERSPECTIVE;
}

// treat mouse inputs to change the camera direction
void Camera::MouseInputs(GLFWwindow* window) {
	int mouseState;
	double posx, posy;
	float rotx, roty;

	mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (firstClick == true) {
		if (mouseState == GLFW_PRESS) {
			glfwGetCursorPos(window, &posx, &posy);
			oldMousePos.x = (float)posx;
			oldMousePos.y = (float)posy;
			firstClick = false;
		}
		return;
	}
	glfwGetCursorPos(window, &posx, &posy);
	rotx = (float)posy - oldMousePos.y;
	roty = (float)posx - oldMousePos.x;

	glm::vec3 newOrientation = glm::rotate(direction, glm::radians(-rotx), glm::normalize(glm::cross(direction, up)));

	// Decides whether or not the next vertical Orientation is legal or not
	if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		direction = newOrientation;

	// Rotates the Orientation left and right
	direction = glm::rotate(direction, glm::radians(-roty), up);
	oldMousePos.x = (float)posx;
	oldMousePos.y = (float)posy;
}
