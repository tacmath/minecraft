#include "scop.h" 

float cameraPosZ = 0;
extern float ObjectSize;

char *getShaderSource(char *fileName) {
    char    *source;
    int     fd;
    size_t  fileLength;

    if ((fd = _open(fileName, O_RDONLY)) <= 0) {
        printf("Failed to open %s\n", fileName);
        return (0);
    }
    fileLength = _lseek(fd, 0, SEEK_END);
    _lseek(fd, 0, SEEK_SET);
    if (!(source = calloc(sizeof(char), fileLength + 1)))
        return (0);
    _read(fd, source, fileLength);
    _close(fd);
    return (source);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        cameraPosZ += 0.2 * ObjectSize;
    else if (yoffset < 0)
        cameraPosZ -= 0.2 * ObjectSize;
}

void printUsage() {
    printf("Usage  : ./scop [OPTION]... FILE.obj\n\n");
    printf("option :\n");
    printf("  -t  TEXTURE_FILE\n");
}

void freeMeshData(t_mesh *mesh) {
    free(mesh->vertices.data);
    free(mesh->indices.data);
    free(mesh->normales.data);
    free(mesh->uvs.data);
    for (int n = 0; n < mesh->segmentNb; n++) {
        free(mesh->segments[n].name);
        free(mesh->segments[n].texture);
    }
    free(mesh->segments);
}

void freeAll(t_scop *scop) {
    glDeleteTextures(1, &scop->background.textureID);
    for (int n = 0; n < scop->object.segmentNb; n++) {
        glDeleteTextures(1, &scop->object.segments[n].textureID);
        glDeleteTextures(1, &scop->object.segments[n].normalTextureID);
    }
    free(scop->object.segments);
}

void limitFPS() {
    static double oldTime = 0;
    static double newTime;

  /*  newTime = glfwGetTime();
    if ((newTime - oldTime) * 1000000 < (1000000.0 / MAX_FPS))
        _usleep((1000000.0 / MAX_FPS) - (newTime - oldTime) * 100000);
    oldTime = glfwGetTime();*/
}