#version 330 core
out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube skybox;

void main()
{
    vec3 color = texture(skybox, texCoord).rgb;
    FragColor = vec4(color, 1.0);
}