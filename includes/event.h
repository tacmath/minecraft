#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>


#define NORMAL_PERSPECTIVE 0
#define UP_PERSPECTIVE 1

class Event {
	GLFWwindow* window;
	glm::dvec2 mousePos;
	char		keyPressed[256];
	bool inMenu;
	float speed;
	float mouseSensitivity;
	bool perspective;

public:
	bool positionChanged;
	bool lookChanged;
	Event();

	void Init(GLFWwindow* window);
	void MovementEvent(Camera& camera);
	void MouseEvent(Camera& camera);
	void KeyEvent();
	void GetEvents(Camera &camera);
}; 

#endif