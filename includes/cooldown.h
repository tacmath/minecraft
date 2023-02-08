#ifndef COOLDOWN_CLASS_H
#define COOLDOWN_CLASS_H

#include <GLFW/glfw3.h>
#include <array>

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

enum COOLDOWN { ACTION, ENUM_END };

class Cooldowns {
private:
	std::array<Cooldown, ENUM_END> cooldown;

public:
	Cooldowns() {
		cooldown[ACTION].Init(true, 0, 0.15f);
	}

	bool Ready(unsigned int id) {
		return cooldown[id].ready;
	}

	bool Use(unsigned int id) {
		if (!cooldown[id].ready)
			return false;
		cooldown[id].ready = false;
		cooldown[id].lastUpdate = (float)glfwGetTime();
		return true;
	}

	void Reset(unsigned int id) {
		cooldown[id].ready = true;
		cooldown[id].lastUpdate = 0;
	}

	void Update() {
		float time = (float)glfwGetTime();

		for (int n = 0; n < cooldown.size(); n++) {
			if (time - cooldown[n].lastUpdate > cooldown[n].downTime)
				cooldown[n].ready = true;
		}
	}
};

#endif