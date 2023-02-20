#version 460
out vec4 FragColor;

in vec3 GtexCoord;
in float Gluminosity;

uniform sampler2DArray atlas;


void main()
{
    FragColor = vec4(vec3(texture(atlas, GtexCoord)) * Gluminosity, 1.0f);
}