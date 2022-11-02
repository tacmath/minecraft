#version 400
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D atlas;

void main()
{
  //  FragColor = texelFetch(dirt, ivec2(texCoord * 16), 0);
    FragColor = texture(atlas, texCoord);
}