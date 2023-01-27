#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include "player.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "debug.h"
#include "world_area.h"
#include "cooldown.h"
#include "window.h"
#include "shadow.h"

#define NORMAL_PERSPECTIVE 0
#define UP_PERSPECTIVE 1

struct ToggleData {
    Debug		*debug;
    Player*		player;
    WorldArea*	worldArea;
	Window*		window;
	Cooldowns	*cooldowns;
	Shadow		*shadow;

	std::function<void(int width, int height)> windowSizeCallback;

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

	void Link(Window* window, Debug *debug, Player *player, WorldArea* worldArea, Cooldowns *cooldowns, Shadow *shadow);
	void GetEvents(float latency);

	void SetWindowSizeCallback(std::function<void(int width, int height)> windowSizeCallback);

private:
	void MovementEvent(float latency);
	void MouseEvent();
	glm::vec3 spectatorMovement();
}; 

#endif