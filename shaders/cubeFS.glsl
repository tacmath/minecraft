#version 330 core
out vec4 FragColor;

//in vec3 color;
in vec2 texCoord;
//in vec3 worldPos;

uniform sampler2D dirt;

void main()
{
    //FragColor = texelFetch(dirt, ivec2(texCoord * 16), 0);
    FragColor = texture(dirt, texCoord); 
}