#version 330 core
out vec4 FragColor;

//in vec3 color;
in vec2 texCoord;
//in vec3 worldPos;

uniform sampler2D dirt;

void main()
{
    FragColor = texture(dirt, texCoord);
}