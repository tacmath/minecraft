#include "application.h"

Block blocks[256];
std::unordered_map<int64_t, Chunk*> chunksMap;
ChunkGeneration     globalChunkGeneration;

#define MIN_FRAME_DELAY 1.0f / MAX_FPS

#define GAME_TPS        20.0f

#define MIN_TICK_DELAY  1.0f / GAME_TPS

Application::Application() {
    status = 0;
}

void Application::Start() {
    // link all the modules dependencies 
    player.Init(minecraft.windowSize);
    debug.Link(&minecraft.windowSize, &player, minecraft.window);
    UI.Link(&minecraft.windowSize);
    event.Link(minecraft.window, &debug, &player, &minecraft);


    UI.InitUniforms(player.camera.projection);
    UI.SetViewMatrix(player.camera.view);
    minecraft.initUniforms(player.camera);

    glfwSwapInterval(0);
    status = APPLICATION_RUNNIG;
}

void Application::Run() {
    float previousLoopTime  = 0;
    float previousFrameTime = 0;
    float previousTickTime  = 0;
    float diff = 0;
    float time = 0;

    while (status) {
        time = (float)glfwGetTime();

        diff = time - previousFrameTime;
        if (diff <= MIN_FRAME_DELAY)
            continue;
        EveryFrames(time, time - previousFrameTime);

        previousFrameTime = time;
        previousLoopTime = time - (diff - MIN_FRAME_DELAY);

        diff = time - previousTickTime;
        if (diff <= MIN_TICK_DELAY)
            continue;
        previousTickTime = time - (diff - MIN_TICK_DELAY);
        EveryTicks();
    }
}

void Application::Stop() {
    minecraft.thread.StopThreads();
}

void Application::EveryFrames(float time, float latency) {
    event.GetEvents(latency);
    if (event.lookChanged || event.positionChanged) {
        minecraft.setChunksVisibility(player.camera);
        minecraft.LoadViewMatrix(player.camera);
        UI.SetViewMatrix(player.camera.view);
    }

    if (glfwGetKey(minecraft.window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwWindowShouldClose(minecraft.window) == 1)
        status = 0;

    minecraft.Draw();
    UI.Draw(player);
    debug.fpsTitle(time, latency);
    debug.Draw(time, latency);

    glfwSwapBuffers(minecraft.window);
}

void Application::EveryTicks() {
    minecraft.LoadChunks(player.position, player.camera);
    minecraft.thread.BindAllChunks();
    minecraft.thread.UnlockLoadedChunks();
}