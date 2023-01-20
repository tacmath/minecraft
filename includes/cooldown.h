#ifndef COOLDOWN_CLASS_H
#define COOLDOWN_CLASS_H

#include <GLFW/glfw3.h>

#define MAX_COOLDOWNS_NB 1

#define ACTION_COOLDOWN 0

struct Cooldown {
public:
	// if the downtime is over
	bool	ready;
	// the last time the cooldown was uptaded
	float	lastUpdate;
	//downtime of the cooldown in seconds
	float	downTime;

	void Init(bool ready, float	lastUpdate, float downTime) {
		this->ready = ready;
		this->lastUpdate = lastUpdate;
		this->downTime = downTime;
	}
};


class Cooldowns {
private:
	Cooldown cooldown[MAX_COOLDOWNS_NB];
public:

	Cooldowns() {
		cooldown[ACTION_COOLDOWN].Init(true, 0, 0.08f);
	}

	bool Ready(int id) {
		return cooldown[id].ready;
	}

	bool Use(int id) {
		if (!cooldown[id].ready)
			return false;
		cooldown[id].ready = false;
		cooldown[id].lastUpdate = (float)glfwGetTime();
		return true;
	}

	void Update() {
		float time = (float)glfwGetTime();

		for (int n = 0; n < MAX_COOLDOWNS_NB; n++) {
			if (time - cooldown[n].lastUpdate > cooldown[n].downTime)
				cooldown[n].ready = true;
		}
	}
};

#endif