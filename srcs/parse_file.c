#include "scop.h" 

void getMeshBorders(t_mesh *mesh) {
    t_vertex *vertices;

    vertices = mesh->vertices.data;
    if (!vertices)
        return;
    mesh->max = vertices[0];
    mesh->min = vertices[0];
    for (int n = 1; n < mesh->vertices.size; n++) {
        if (vertices[n].x > mesh->max.x)
            mesh->max.x = vertices[n].x;
        if (vertices[n].y > mesh->max.y)
            mesh->max.y = vertices[n].y;
        if (vertices[n].z > mesh->max.z)
            mesh->max.z = vertices[n].z;
        if (vertices[n].x < mesh->min.x)
            mesh->min.x = vertices[n].x;
        if (vertices[n].y < mesh->min.y)
            mesh->min.y = vertices[n].y;
        if (vertices[n].z < mesh->min.z)
            mesh->min.z = vertices[n].z;
    }
}

void getIndices(FILE *file, t_mesh *mesh) {
    t_indice  indices[4];
    char    buffer[4][30];
    char    line[256];
    int ret;

    if (!mesh->indices.size && !mesh->segmentNb) {
        mesh->segmentNb = 1;
        mesh->segments = calloc(1, sizeof(t_usemtl));
        mesh->segments->start = 0;
    }
    fscanf(file, "%[^\n]", line);
    if ((ret = sscanf(line, "%s %s %s %s\n", buffer[0], buffer[1], buffer[2], buffer[3])) < 3)
        return;
    for (int n = 0; n < ret; n++)
        sscanf(buffer[n], "%d/%d/%d", &(indices[n].vertex), &(indices[n].uvs), &(indices[n].normale));
    mesh->indices.data = realloc(mesh->indices.data, sizeof(t_indice) * (mesh->indices.size + 3 + (3 * (ret == 4))));
    memcpy(&((t_indice*)mesh->indices.data)[mesh->indices.size], indices, sizeof(t_indice) * 3);
    mesh->indices.size += 3;
    if (ret == 4) {
        memcpy(&((t_indice*)mesh->indices.data)[mesh->indices.size], indices, sizeof(t_indice) * 3);
        ((t_indice*)mesh->indices.data)[mesh->indices.size + 1] = indices[3];
        mesh->indices.size += 3;
    }
}

void getVertices(FILE *file, t_array *vertices) {
    t_vertex    vertex;

    vertices->data = realloc(vertices->data, sizeof(t_vertex) * (vertices->size + 1));
    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
    ((t_vertex*)vertices->data)[vertices->size++] = vertex;
}

void getUvs(FILE *file, t_mesh *mesh) {
    t_vec2  uv;

    mesh->uvs.data = realloc(mesh->uvs.data, sizeof(t_vec2) * (mesh->uvs.size + 1));
    fscanf(file, "%f %f\n", &uv.x, &uv.y);
    ((t_vec2*)mesh->uvs.data)[mesh->uvs.size++] = uv;
}
/*
void printObjectData(t_mesh *mesh) {
    GLuint *indices = mesh->indices.data;
    t_vertex *vertices = mesh->vertices.data;
    t_vertex *normales = mesh->normales.data;
    t_vec2  *uvs =mesh->uvs.data;

    printf("Vertices :\n\n");
    for (int n = 0; n < mesh->vertices.size; n++)
        printf("v = (%7.4f, %7.4f, %7.4f)\n", vertices[n].x, vertices[n].y, vertices[n].z);
    printf("\nUvs :\n\n");
    for (int n = 0; n < mesh->uvs.size; n++)
        printf("uv = %7.4f, %7.4f\n", uvs[n].x, uvs[n].y);
    printf("\nNormales :\n\n");
    for (int n = 0; n < mesh->normales.size; n++)
        printf("n = (%7.4f, %7.4f, %7.4f)\n", normales[n].x, normales[n].y, normales[n].z);
    printf("\nIndices :\n\n");
    for (int n = 0; n < (mesh->indices.size / 3); n++)
        printf("indices = %6d, %6d, %6d\n", indices[n * 3], indices[n * 3 + 1], indices[n * 3 + 2]);
    printf("\nmin = (%7.4f, %7.4f, %7.4f)\n", mesh->min.x, mesh->min.y, mesh->min.z);
    printf("max = (%7.4f, %7.4f, %7.4f)\n", mesh->max.x, mesh->max.y, mesh->max.z);
}*/

void printSegments(t_mesh *mesh) {
    for (int n = 0; n < mesh->segmentNb; n++) {
        printf("\nname = %s\n", mesh->segments[n].name);
        printf("texture = %s\n", mesh->segments[n].texture);
        printf("start = %d\n", mesh->segments[n].start);
        printf("size = %d\n", mesh->segments[n].size);
    }
}

int indexMeshData(t_mesh *mesh) {
    t_vertex *newVertices;
    t_vertex *newNormales;
    t_vec2   *newUvs;
    t_indice  *indices;

    indices = mesh->indices.data;
    if (!(newVertices = malloc(sizeof(t_vertex) * mesh->indices.size)) ||
        !(newNormales = malloc(sizeof(t_vertex) * mesh->indices.size)) ||
        !(newUvs = malloc(sizeof(t_vec2) * mesh->indices.size)))
        return (0);
    for (int n = 0; n < mesh->indices.size; n++) {
        newVertices[n] = ((t_vertex*)mesh->vertices.data)[indices[n].vertex - 1];       // faire gaffe ca va crash si l'index est faux
        newUvs[n] = ((t_vec2*)mesh->uvs.data)[indices[n].uvs - 1];
        newNormales[n] = ((t_vertex*)mesh->normales.data)[indices[n].normale - 1];
    }
    free(mesh->vertices.data);
    free(mesh->normales.data);
    free(mesh->uvs.data);
    mesh->vertices.data = newVertices;
    mesh->normales.data = newNormales;
    mesh->uvs.data = newUvs;
    mesh->vertices.size = mesh->indices.size;
    mesh->uvs.size = mesh->indices.size;
    mesh->normales.size = mesh->indices.size;
    mesh->indices.size = 0;
    free(mesh->indices.data);
    mesh->indices.data = 0;
    return (1);
}

int parseIndices(t_mesh *mesh) {
    GLuint *newIndices;
    t_indice  *indices;

    if (!mesh->normales.size || !mesh->uvs.size) {
        if (!(newIndices = malloc(sizeof(GLuint) * mesh->indices.size)))
            return (0);
        indices = mesh->indices.data;
        for (int n = 0; n < mesh->indices.size; n++)
            newIndices[n] = indices[n].vertex - 1;
        free(mesh->indices.data);
        mesh->indices.data = newIndices;
        return (1);
    }
    indexMeshData(mesh);
    return (1);
}

void parseMtllib(t_mesh *mesh, char *path) {
    FILE *file;
    char *fileName;
    char buffer[256];
    int segmentIndex;

    if (!mesh->mltFile)
        return ;
    if ((fileName = strrchr(path, '/'))) {
        fileName[1] = 0;
        fileName = ft_strjoin(path, mesh->mltFile);
        free(mesh->mltFile);
    }
    else
        fileName = mesh->mltFile;
    if (!(file = fopen(fileName, "r")))
        return ;
    free(fileName);
    segmentIndex = -1;
    while (fscanf(file, "%s", buffer) != EOF) {
        if (!strcmp(buffer, "newmtl")) {
            segmentIndex = -1;
            fscanf(file, "%s\n", buffer);
            for (int n = 0; n < mesh->segmentNb; n++) {
                if (mesh->segments[n].name && !strcmp(buffer, mesh->segments[n].name)) {
                    segmentIndex = n;
                    break ;
                }
            }
        }
        else if (!strcmp(buffer, "map_Kd")) {
            fscanf(file, "%s\n", buffer);
            if (segmentIndex != -1)
                mesh->segments[segmentIndex].texture = ft_strjoin(path, buffer);
        }
    }
    fclose(file);
}

void addSegment(FILE *file, t_mesh *mesh) {
    char buffer[256];
    char name[256];

    fscanf(file, "%[^\n]", buffer);
    if (sscanf(buffer, "%s", name) < 1)
        return ;
    mesh->segments = realloc(mesh->segments, sizeof(t_usemtl) * (mesh->segmentNb + 1));
    mesh->segments[mesh->segmentNb].name = _strdup(name);
    mesh->segments[mesh->segmentNb].start = mesh->indices.size;
    mesh->segments[mesh->segmentNb].texture = 0;
    mesh->segmentNb++;
}
void parseSegments(t_mesh *mesh) {
    for (int n = 0; n < mesh->segmentNb - 1; n++)
        mesh->segments[n].size = mesh->segments[n + 1].start - mesh->segments[n].start;
    mesh->segments[mesh->segmentNb - 1].size = mesh->indices.size - mesh->segments[mesh->segmentNb - 1].start;
}

int getObjectData(t_mesh *mesh, char *fileName) {
    FILE *file;
    char buffer[256];

    if (!(file = fopen(fileName, "r")))
        return (0);
    while (fscanf(file, "%s", buffer) != EOF) {
        if (!strcmp(buffer, "v"))
            getVertices(file, &mesh->vertices);
        else if (!strcmp(buffer, "vt"))
            getUvs(file, mesh);
        else if (!strcmp(buffer, "vn"))
            getVertices(file, &mesh->normales);
        else if (!strcmp(buffer, "f"))
            getIndices(file, mesh);
        else if (!strcmp(buffer, "mtllib")) {
            fscanf(file, "%s\n", buffer);
            mesh->mltFile = _strdup(buffer);
        }
        else if (!strcmp(buffer, "usemtl"))
            addSegment(file, mesh);
        else
            fscanf(file, "\n");
    }
    fclose(file);
    getMeshBorders(mesh);
    parseSegments(mesh);
    parseMtllib(mesh, fileName);
    if (!parseIndices(mesh))
        return (0);
    //printSegments(mesh);
    //printObjectData(mesh);
    return (1);
}