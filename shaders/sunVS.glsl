#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 texCoord;

uniform int phase;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{   
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    gl_Position.z = gl_Position.w;

    texCoord = vec3(aTex, phase);
}