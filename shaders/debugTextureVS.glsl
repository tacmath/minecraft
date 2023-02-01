#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoords;

uniform mat4 matrix;

void main()
{   
    gl_Position = matrix * vec4(aPos, 0.1f);
    TexCoords = aTex;
}