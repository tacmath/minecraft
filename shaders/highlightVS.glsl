#version 400
layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 cubePos;

void main()
{
   gl_Position = projection * view * vec4(vertex.x + cubePos.x, vertex.y + cubePos.y, vertex.z + cubePos.z, 1.0f);
   gl_Position.z -= 0.002f;
}