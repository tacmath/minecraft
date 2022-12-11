#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include "player.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "debug.h"
#include "minecraft.h"

#define NORMAL_PERSPECTIVE 0
#define UP_PERSPECTIVE 1

class Event {
	GLFWwindow* window;
	Player*		player;
	glm::dvec2	mousePos;
	bool		inMenu;
	float		speed;
	float		mouseSensitivity;
	bool		perspective;

	// Eular Angles
	float Yaw;
	float Pitch;

public:
	bool positionChanged;
	bool lookChanged;
	float frequence;

	Event();

	~Event();

	void Init(GLFWwindow* window, Debug *debug, Player *player, Minecraft* minecraft);
	void MovementEvent();
	void MouseEvent();
	void GetEvents();
private:
	void removePointedCube();
	void placeCube();
	glm::vec3 spectatorMovement();
}; 

#endif