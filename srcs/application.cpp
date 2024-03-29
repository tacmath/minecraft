#include "application.h"

Application::Application() {
    status = 0;
}

void Application::Start() {

    // link all the modules dependencies 
    player.Init(window.size);
    worldArea.Init(player.camera, sound);
    menu.Link(&player, &window, &worldArea, &shadow, &event.mouseSensitivity);
    event.Link(&window, &menu, &player, &cooldowns);
    shadow.Link(window.context, &player.camera, &worldArea);


    UI.InitUniforms(player.camera.projection);
    UI.SetViewMatrix(player.camera.view);
    background.initUniforms(player.camera);

    
    shadow.Activate();
    SetCallbacks();
    player.InventoryUpdateCallback();
    ImGui_ImplGlfw_InstallCallbacks(window.context); //maybe add imgui callbacks directly in my callbacks
    glfwSwapInterval(1);
    status = APPLICATION_RUNNING;
}

void Application::Run() {
    float previousFrameTime = 0; // TODO need to remake for low FPS
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
            previousFrameTime = time/* - (diff - MIN_FRAME_DELAY)*/;
        }
    }
}

void Application::Stop() {
    worldArea.thread.StopThreads();
    for (int n = 0; n < MAX_BLOCK_NB; n++)
        Chunk::blocks[n].Delete();
}

void Application::EveryFrames(float time, float latency) {
    (void)time;
    cooldowns.Update();
    event.GetEvents(latency);
    if (glfwWindowShouldClose(window.context) == 1)
        status = 0;

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    worldArea.Draw();
    background.Draw();
    UI.Draw(player);
    menu.Draw();

    //   menu.fpsTitle(time, latency);

    glfwSwapBuffers(window.context);
}

void Application::EveryTicks() {
    worldArea.LoadChunks(player.position, player.camera); // place in a callback called when player change chunk
    worldArea.thread.BindAllChunks();
    worldArea.thread.UnlockLoadedChunks();
    background.sun.tick();
}

void Application::SetCallbacks() {
    player.SetUpdateCallback([&](const Player &player) { //maybe set the callback in the camera class
        worldArea.setChunksVisibility(player.camera);
        worldArea.LoadViewMatrix(player.camera);
        background.LoadViewMatrix(player.camera);
        UI.SetViewMatrix(player.camera.view);
     });

    player.SetInventoryUpdateCallback([&](const Player& player) { //maybe set the callback in the camera class
        UI.UpdateInventory(player);
        int width, height;
        glfwGetFramebufferSize(window.context, &width, &height);
        glViewport(0, 0, width, height);
    });

    event.SetWindowSizeCallback([&](int width, int height) {
        player.camera.ChangePerspective(0, (float)width, (float)height, 0.1f, 0);
        worldArea.initUniforms(player.camera);
        background.initUniforms(player.camera);
        UI.InitUniforms(player.camera.projection);
     });

    menu.SetUpdatPerspectiveCallback([&](void) { //maybe set the callback in the camera class
            worldArea.setChunksVisibility(player.camera);
            worldArea.initUniforms(player.camera);
            background.initUniforms(player.camera);
            UI.InitUniforms(player.camera.projection);
            background.sun.UpdateCallback();
    });

    menu.SetOnExitCallback([&](void) {
        status = 0;
    });

    background.sun.SetUpdateCallback([&](glm::vec3 &sunPosition) {
        Shader& chunkShader = worldArea.GetShader();
        chunkShader.setFloat("dayLightLevel", (1.0f - (1.0f - glm::smoothstep(0.0f, 0.5f, sunPosition.y)) * 0.7f));
        
        background.LoadSunPos(sunPosition);

        glm::vec3 position = sunPosition;
        if (sunPosition.y < 0)
            position = -position;
        chunkShader.setVec3("lightDir", position);
        chunkShader.setFloat("timeCycle", glm::smoothstep(0.05f, 0.4f, position.y));

        if (position.y < 1)
            shadow.GenerateShadowMap(position);
    });
}