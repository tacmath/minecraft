#include "minecraft.h"
#include "EBO.h"


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

void Minecraft::initChunks(int radius) {
    long diameter = (radius + 1) * 2;

    chunksLoading.resize(diameter * diameter);
    for (int x = 0; x < diameter; x++)
        for (int z = 0; z < diameter; z++) {
            Chunk *newChunk = new Chunk;
            newChunk->SetPosistion(x - (radius + 1), z - (radius + 1));
            chunksMap[GET_CHUNK_ID(newChunk->posx, newChunk->posz)] = newChunk;
            newChunk->Generate();
            chunksLoading[x * diameter + z] = newChunk;
        }
    for (int x = 1; x < diameter - 2; x++)
        for (int z = 1; z < diameter - 2; z++) {
            Chunk* chunk = chunksLoading[x * diameter + z];
            chunk->createMeshData();
            chunk->Bind();
            chunk->isVisible = camera.frustum.chunkIsVisible(chunk->posx, chunk->posz);
            chunks.push_back(chunk);
            chunksLoading.erase(chunksLoading.begin() + x * diameter + z);
        }
}

void Minecraft::initUniforms(void) {
    chunkShader.Activate();
    chunkShader.setInt("atlas", 0);
    chunkShader.setMat4("projection", camera.projection);
    chunkShader.setMat4("view", camera.view);

    skyboxShader.Activate();
    skyboxShader.setInt("skybox", 0);
    skyboxShader.setMat4("projection", camera.projection);
    skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.view)));
}

void Minecraft::enableGlParam(void) {
//    glDisable(GL_MULTISAMPLE);      // deactivate multisample to avoid weird texture problem with the atlas
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Minecraft::initWindows(void) {
    char path[1024];

    cwd(path, 1024);
    if (!glfwInit()) {
        printf("Glfw failed to init\n");
        exit(-1);
    }
    cd(path);
//    glfwWindowHint(GLFW_SAMPLES, 4);      // no multisampling
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Pour rendre MacOS heureux ; ne devrait pas �tre n�cessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minecraft", NULL, NULL))) {
        glfwTerminate();
        printf("Glfw failed to create a window\n");
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Minecraft::initSkybox(void) {
    const char** texturesName;

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

    std::vector <GLuint> indices(skyboxIndices, skyboxIndices + sizeof(skyboxIndices) / sizeof(GLuint));
    EBO<GLuint> skyboxEBO(indices);

    texturesName = new const char* [6];
    texturesName[0] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_2_Left+X.png";
    texturesName[1] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_3_Right-X.png";
    texturesName[2] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_4_Up+Y.png";
    texturesName[3] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_5_Down-Y.png";
    texturesName[4] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_0_Front+Z.png";
    texturesName[5] = "texture/TXR_ENV_Skybox_Cloud Layers__Cam_1_Back-Z.png";
    
    skyboxCubemap.Gen(0);
    skyboxCubemap.Load(texturesName);
    delete[] texturesName;

    skybox.Gen();
    skybox.LinkAttrib((void*)skyboxVertices, 8, 0, 3, GL_FLOAT, sizeof(float), (void*)0);
    skybox.LinkAttrib((void*)texturesUv, 8, 1, 2, GL_FLOAT, sizeof(float), (void*)0);
    skybox.Bind();
    skyboxEBO.Bind();
}