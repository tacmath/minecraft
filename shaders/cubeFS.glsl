#version 400
out vec4 FragColor;

in vec3 texCoord;
in float luminosity;
//in float dist;

uniform sampler2DArray atlas;


void main()
{
    /*if (dist > 160)
        discard;*/
    FragColor = vec4(vec3(texture(atlas, texCoord)) * luminosity, 1.0f);
//    FragColor = vec4(vec3(1.0f) * luminosity, 1.0f);
}