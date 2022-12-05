#version 400
out vec4 FragColor;

in vec3	normal;
in vec3 texCoord;
in vec3 fragPos;
in float luminosity;

uniform vec3 lightDir;
uniform sampler2DArray atlas;

void main()
{
    FragColor = vec4(vec3(texture(atlas, texCoord)) * luminosity, 1.0f);
}