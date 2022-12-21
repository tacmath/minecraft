#include "application.h"

Block blocks[256];
std::unordered_map<int64_t, Chunk*> chunksMap;
ChunkGeneration     globalChunkGeneration;

Application::Application() {
    status = 0;
}

void Application::Start() {
    // link all the modules dependencies 
    player.Init(window.size);
    debug.Link(&window.size, &player, window.context);
    UI.Link(&window.size);
    event.Link(window.context, &debug, &player, &minecraft);


    UI.InitUniforms(player.camera.projection);
    UI.SetViewMatrix(player.camera.view);
    minecraft.initUniforms(player.camera);
    background.initUniforms(player.camera);

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

        diff = time - previousTickTime;
        if (diff >= MIN_TICK_DELAY) {
            EveryTicks();
            previousTickTime = time - (diff - MIN_TICK_DELAY);
        }

        diff = time - previousFrameTime;
        if (diff >= MIN_FRAME_DELAY) {
            EveryFrames(time, time - previousFrameTime);
            previousFrameTime = time;
            previousLoopTime = time - (diff - MIN_FRAME_DELAY);
        } /*else {
            sleep for the remaining time
        }*/
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
        background.LoadViewMatrix(player.camera);
        UI.SetViewMatrix(player.camera.view);
    }

    if (glfwGetKey(window.context, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwWindowShouldClose(window.context) == 1)
        status = 0;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    background.Draw();
    minecraft.Draw();
    UI.Draw(player);
    debug.fpsTitle(time, latency);
    debug.Draw(time, latency);

    glfwSwapBuffers(window.context);
}

void Application::EveryTicks() {
    minecraft.LoadChunks(player.position, player.camera);
    minecraft.thread.BindAllChunks();
    minecraft.thread.UnlockLoadedChunks();
}