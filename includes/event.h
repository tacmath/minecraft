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
#include "cooldown.h"

#define NORMAL_PERSPECTIVE 0
#define UP_PERSPECTIVE 1

struct ToggleData {
    Debug *debug;
    Player* player;
    Minecraft* minecraft;

    bool* perspective;
    bool* lookChanged;
};

class Event {
	GLFWwindow* window;
	Player*		player;
	Cooldowns*	cooldowns;
	glm::dvec2	mousePos;
	bool		inMenu;
	float		speed;
	float		mouseSensitivity;
	bool		perspective;

	// Eular Angles
	float Yaw;
	float Pitch;

	bool playerUpdated;

public:

	Event();

	~Event();

	void Link(GLFWwindow* window, Debug *debug, Player *player, Minecraft* minecraft, Cooldowns *cooldowns);
	void GetEvents(float latency);

private:
	void MovementEvent(float latency);
	void MouseEvent();
	void removePointedCube();
	void placeCube();
	glm::vec3 spectatorMovement();
}; 

#endif