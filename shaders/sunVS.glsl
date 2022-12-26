#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;

uniform vec3 position;
uniform mat4 projection;
uniform mat4 view;

void main()
{   
    gl_Position = projection * view * vec4(aPos + position, 1.0f);
//    gl_Position /= gl_Position.w;

    gl_Position.z = gl_Position.w;
    texCoord = aTex;
}