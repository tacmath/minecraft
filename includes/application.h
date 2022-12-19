#ifndef APPLICATION_H
# define APPLICATION_H

#include "minecraft.h"
#include "player.h"
#include "debug.h"
#include "UI.h"
#include "event.h"

#include "chunk_generation.h"
#include "blocks.h"

#define APPLICATION_RUNNIG 1

class Application {
private:
	int				status;
	Minecraft		minecraft;
	Player          player;
	Debug           debug;
	UserInterface   UI;
	Event           event;

public:
	//default constructor
    Application();

	//initialize all modules needed
    void Start();

	//run the application
	void Run();

	// properly stop the application
	void Stop();

private:
	// called on each frame with the time of the application the latency since last frame in seconds
	void EveryFrames(float time, float latency);

	// called on every game tick
	void EveryTicks();
};

#endif