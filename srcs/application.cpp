#include "application.h"

Application::Application() {
    status = 0;
}

void Application::Start() {
    // link all the modules dependencies 
    player.Init(window.size);
    debug.Link(&window.size, &player, window.context);
    UI.Link(&window.size);
    event.Link(&window, &debug, &player, &worldArea, &cooldowns, &shadow);
    shadow.Link(window.context, &player.camera, &worldArea);


    UI.InitUniforms(player.camera.projection);
    UI.SetViewMatrix(player.camera.view);
    worldArea.initUniforms(player.camera);
    background.initUniforms(player.camera);

    shadow.Activate();
    SetCallbacks();
    glfwSwapInterval(0);
    status = APPLICATION_RUNNING;
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
    worldArea.thread.StopThreads();
}

void Application::EveryFrames(float time, float latency) {
    cooldowns.Update();
    event.GetEvents(latency);
    if (glfwGetKey(window.context, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwWindowShouldClose(window.context) == 1)
        status = 0;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    worldArea.Draw();
    background.Draw();
    UI.Draw(player);
    debug.fpsTitle(time, latency);
    debug.Draw(time, latency);

    glfwSwapBuffers(window.context);
}

void Application::EveryTicks() {
    worldArea.LoadChunks(player.position, player.camera); // place in a callback called when player change chunk
    worldArea.thread.BindAllChunks();
    worldArea.thread.UnlockLoadedChunks();
    background.sun.tick();
}

void Application::SetCallbacks() {
    player.SetUpdateCallback([&](Player &player) {
        worldArea.setChunksVisibility(player.camera);
        worldArea.LoadViewMatrix(player.camera);
        background.LoadViewMatrix(player.camera);
        UI.SetViewMatrix(player.camera.view);
     });

    event.SetWindowSizeCallback([&](int width, int height) {
        player.camera.ChangePerspective(80, (float)width, (float)height, 0.1f, 24.0f * RENDER_DISTANCE);
        worldArea.initUniforms(player.camera);
        background.initUniforms(player.camera);
        UI.InitUniforms(player.camera.projection);
     });

    background.sun.SetUpdateCallback([&](glm::vec3 &sunPosition) {
        Shader& chunkShader = worldArea.GetShader();
        chunkShader.Activate();
        chunkShader.setFloat("day", glm::smoothstep(0.0f, 0.5f, sunPosition.y));

        glm::vec3 position = sunPosition;
        if (sunPosition.y < 0)
            position = -position;
        chunkShader.setVec3("lightDir", position);
        chunkShader.setFloat("timeCycle", glm::smoothstep(0.05f, 0.4f, position.y));

        if (position.y < 1)
            shadow.GenerateShadowMap(position);
    });
}