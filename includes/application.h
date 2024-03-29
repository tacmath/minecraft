#ifndef APPLICATION_H
# define APPLICATION_H

#include "window.h"
#include "world_area.h"
#include "background.h"
#include "player.h"
#include "menu.h"
#include "UI.h"
#include "event.h"
#include "cooldown.h"
#include "shadow.h"
#include "sound.h"

#include "generation.h"
#include "blocks.h"

# define MAX_FPS        6000

#define MIN_FRAME_DELAY 1.0f / MAX_FPS

#define GAME_TPS        20.0f

#define MIN_TICK_DELAY  1.0f / GAME_TPS

#define APPLICATION_RUNNING 1

class Application {
private:
	int				status;

	// modules
	Window			window;
	WorldArea		worldArea;
	Background		background;
	Player          player;
	Menu			menu;
	UserInterface   UI;
	Event           event;
	Cooldowns		cooldowns;
	Shadow			shadow;
	Sound			sound;

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
	// set modules callbacks
	void SetCallbacks();

	// called on each frame with the time of the application the latency since last frame in seconds
	void EveryFrames(float time, float latency);

	// called on every game tick
	void EveryTicks();
};

#endif