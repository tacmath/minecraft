#include "minecraft.h"

#ifdef _WIN32
#include <direct.h>
// MSDN recommends against using getcwd & chdir names
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

float skyboxVertices[] =
{
    //   Coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
     1.0f, -1.0f,  1.0f,//       /|       /|
     1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
     1.0f,  1.0f,  1.0f,//      |/       |/
     1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
};

float texturesUv[] =
{
    //   Coordinates
    0.0f,0.0f,
    0.0f,1.0f,
    0.0f,0.0f,
    0.0f,1.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    1.0f,0.0f,
    1.0f,1.0f
};

unsigned int skyboxIndices[] =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};

void showFPS(GLFWwindow* window) {
    static double oldTime = 0;
    static double newTime;
    static int frames = 0;
    static char title[40];
    double timeDiff;

    newTime = glfwGetTime();
    timeDiff = newTime - oldTime;
    frames++;
    if (timeDiff < 1.0f / 30.0f)
        return;
    sprintf(title, "scop :  FPS = %d  ms = %f", (int)((1.0 / timeDiff) * frames), (timeDiff * 1000) / frames);
    glfwSetWindowTitle(window, title);
    frames = 0;
    oldTime = newTime;
}

void getMouseEvent(GLFWwindow* window, Shader cube, Shader sky) {
    int state;
    static double prevPosx = -1.0f;
    static double prevPosy = -1.0f;
    double posx, posy;
    static float  totalRotX = 0.0;
    static float  totalRotY = 0.0;

    state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(window, &posx, &posy);
    if (state == GLFW_PRESS && prevPosx != -1.0f && prevPosy != -1.0f)
    {
        glm::mat4 matrix(1.0f);
        float  rotationY;

        totalRotY += (posx - prevPosx) / 2;
        if (fabs(totalRotX + ((posy - prevPosy) / 2)) < 90)
            totalRotX += (posy - prevPosy) / 2;
        rotationY = totalRotY * (PI / 180.0f);
        matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -5.0f));
        matrix = glm::rotate(matrix, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(totalRotX), glm::vec3(cos(rotationY), 0.0f, sin(rotationY)));
        cube.Activate();
        cube.setMat4("view", matrix);
        sky.Activate();
        matrix = glm::mat4(glm::mat3(matrix));
        sky.setMat4("view", matrix);
    }
    prevPosx = posx;
    prevPosy = posy;
}

void loop(GLFWwindow* window, Shader cube) {
    Shader skyShader("shaders/skyBoxVS.glsl", "shaders/skyBoxFS.glsl");
    glm::mat4 proj(1.0f);
    glm::mat4 view(1.0f);

    
    proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    cube.Activate();
    cube.setMat4("projection", proj);

    skyShader.Activate();
    skyShader.setMat4("projection", proj);
    skyShader.setMat4("view", view);

    view = glm::translate(view, glm::vec3(0.0, 0.0, -5.0));
    cube.Activate();
    cube.setMat4("view", view);
    
    skyShader.setInt("skybox", 0);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glfwSwapInterval(0);
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();
        getMouseEvent(window, cube, skyShader);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL);
        skyShader.Activate();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthFunc(GL_LESS);


        glEnable(GL_CULL_FACE);
        cube.Activate();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDisable(GL_CULL_FACE);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glfwSwapBuffers(window);
        showFPS(window);
    }
}

GLFWwindow *initWindows(void) {
    GLFWwindow* window;
    char path[1024];

    cwd(path, 1024);
    if (!glfwInit()) {
        printf("Glfw failed to init\n");
        return (0);
    }
    cd(path);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minecraft", NULL, NULL))) {
        glfwTerminate();
        printf("Glfw failed to create a window\n");
        return (0);
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    return (window);
}



int main(void) {
    GLFWwindow* window;
    window = initWindows();
    std::vector <GLuint> ind(skyboxIndices, skyboxIndices + sizeof(skyboxIndices) / sizeof(GLuint));
    EBO<GLuint> testvbo(ind);
    Texture texDirt("texture/dirt.png", 0);
    Shader cubeShader("shaders/cubeVS.glsl", "shaders/cubeFS.glsl");
    VAO test;
    const char** texturesName;

    texturesName = new const char*[6];
    texturesName[0] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_2_Left+X.png";
    texturesName[1] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_3_Right-X.png";
    texturesName[2] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_4_Up+Y.png";
    texturesName[3] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_5_Down-Y.png";
    texturesName[4] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_0_Front+Z.png";
    texturesName[5] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_1_Back-Z.png";
    CubeMap skyboxtex(texturesName, 0);


    test.LinkAttrib((void*)skyboxVertices, 8, 0, 3, GL_FLOAT, sizeof(float), (void*)0);
    test.LinkAttrib((void*)texturesUv, 8, 1, 2, GL_FLOAT, sizeof(float), (void*)0);
    test.Bind();
    testvbo.Bind();
    cubeShader.Activate();
    cubeShader.setInt("dirt", 0);
    loop(window, cubeShader);
    cubeShader.Delete();
    test.Delete();
	return (0);
}