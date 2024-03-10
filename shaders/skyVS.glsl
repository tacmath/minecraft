#version 400
layout (location = 0) in vec2 aPos;

out vec3 fragCoord;

uniform mat4 IVP;

void main()
{   
    gl_Position = vec4(aPos, 1.0f, 1.0f);
    vec4 coord = IVP * vec4(aPos, 0.0f, 1.0f);
    fragCoord = normalize(vec3(coord) / coord.w); // vec3(coord) might be enougth 
}