#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;

uniform mat4 MVP;

void main()
{   
    gl_Position = MVP * vec4(aPos, 1.0f);

    texCoord = aTex;
}