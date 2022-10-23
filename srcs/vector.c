#include "scop.h" 

void normalise(t_vertex *vector) {
    float normal = sqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);

    vector->x /= normal;
    vector->y /= normal;
    vector->z /= normal;
}

t_vertex vec3sub(t_vertex v1, t_vertex v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return (v1);
}

t_vec2 vec2sub(t_vec2 v1, t_vec2 v2) {
    v1.x -= v2.x;
    v1.y -= v2.y;
    return (v1);
}