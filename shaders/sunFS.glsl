#version 400
out vec4 FragColor;

in vec3 texCoord;

uniform sampler2DArray sunTexture;

void main()
{
    FragColor = texture(sunTexture, texCoord);
}