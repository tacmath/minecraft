#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include "player.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "menu.h"
#include "world_area.h"
#include "cooldown.h"
#include "window.h"
#include "shadow.h"
#include <functional>

struct GlfwCallbackData {
    Menu		*menu;
    Player*		player;
    WorldArea*	worldArea;
	Window*		window;
	Cooldowns	*cooldowns;

	std::function<void(int width, int height)> windowSizeCallback;

    bool* lookChanged;
};

class Event {
	GLFWwindow* window;
	Player*		player;
	Cooldowns*	cooldowns;
	Menu*		menu;
	glm::dvec2	mousePos;
	bool		inMenu;
	float		speed;
	float		mouseSensitivity;

	// Eular Angles
	float Yaw;
	float Pitch;

	bool playerUpdated;

public:

	Event();

	~Event();

	void Link(Window* window, Menu *menu, Player *player, WorldArea* worldArea, Cooldowns *cooldowns);
	void GetEvents(float latency);

	void SetWindowSizeCallback(std::function<void(int width, int height)> windowSizeCallback);

private:
	void MovementEvent(float latency);
	void MouseEvent();
	glm::vec3 spectatorMovement();
}; 

#endif