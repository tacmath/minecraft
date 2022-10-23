#include "scop.h" 

void printMat4(t_mat4 mat4) {
    printf("\n%4f %4f %4f %4f\n", mat4[0][0], mat4[0][1], mat4[0][2], mat4[0][3]);
    printf("%4f %4f %4f %4f\n", mat4[1][0], mat4[1][1], mat4[1][2], mat4[1][3]);
    printf("%4f %4f %4f %4f\n", mat4[2][0], mat4[2][1], mat4[2][2], mat4[2][3]);
    printf("%4f %4f %4f %4f\n", mat4[3][0], mat4[3][1], mat4[3][2], mat4[3][3]);
}

void mat4Mult(t_mat4 mat1, t_mat4 mat2, t_mat4 *result) {
    float res;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            res = 0;
            for (int n = 0; n < 4; n++)
                res += mat1[x][n] * mat2[n][y];
            (*result)[x][y] = res;
        }
    }
}

void mat4Traslate(t_mat4 *matrice, t_vertex vector) {
    (*matrice)[0][3] = vector.x;
    (*matrice)[1][3] = vector.y;
    (*matrice)[2][3] = vector.z;
}

t_vertex mat4Vec3Mult(t_mat4 mat, t_vertex vec) {
    t_vertex res;

    res.x= mat[0][0] * vec.x + mat[0][1] * vec.y + mat[0][2] * vec.z;
    res.y= mat[1][0] * vec.x + mat[1][1] * vec.y + mat[1][2] * vec.z;
    res.z= mat[2][0] * vec.x + mat[2][1] * vec.y + mat[2][2] * vec.z;
    return (res);
}

void mat4Transpose(t_mat4 *matrix) {
    float tmp;

    for (int m = 0; m < 3; m++)
        for (int n = 1 + m; n < 4; n++)
        {
            tmp = (*matrix)[n][m];
            (*matrix)[n][m] = (*matrix)[m][n];
            (*matrix)[m][n] = tmp;
        }
}

void mat4SetIdentity(t_mat4 *matrix) {
    t_mat4 tmp = IDENTITY_MAT4;
    memcpy(matrix, tmp, sizeof(t_mat4));
}

void mat4Scale(t_mat4 *result, t_vertex vector) {
    t_mat4 matrice = IDENTITY_MAT4;
    matrice[0][0] = vector.x;
    matrice[1][1] = vector.y;
    matrice[2][2] = vector.z;
    memcpy(result, &matrice, sizeof(t_mat4));
}

void perspective(float fov, float ratio, float near, float far, t_mat4 *result) {
    float   scale = 1.0f / tanf((fov / 2.0f) * (PI / 180.0f));
    t_mat4 matrix = {{scale / ratio,    0,      0,      0}
                    ,{0,                scale,  0,      0}
                    ,{0,                0,      -((far + near) / (far - near)),   -((2 * far * near) / (far - near))}
                    ,{0,                0,     -1,      0}};
    memcpy(result, &matrix, sizeof(t_mat4));
}

void rotate(t_mat4 matrix, float angle, t_vertex vector, t_mat4 *result) {
    float c = cosf((PI / 180.0f) * angle);
    float s = sinf((PI / 180.0f) * angle);
    normalise(&vector);
    float x = vector.x;
    float y = vector.y;
    float z = vector.z;
    float rc = 1 - c;
    t_mat4 rmatrix =    {{x*x*rc+c,     x*y*rc-z*s, x*z*rc+y*s, 0}
                        ,{y*x*rc+z*s,   y*y*rc+c,   y*z*rc-x*s, 0}
                        ,{x*z*rc-y*s,   y*z*rc+x*s, z*z*rc+c,   0}
                        ,{0,            0,          0,          1}};
    mat4Mult(matrix, rmatrix, result);
}