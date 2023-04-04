#version 400
out vec4 FragColor;

in vec2 texCoord;

#ifndef ARRAY
uniform sampler2D Texture;
#else
uniform sampler2DArray Texture;
uniform int texIndex;
#endif

void main()
{
	#ifndef ARRAY
	FragColor = texture(Texture, texCoord);
	#else
	FragColor = texture(Texture, vec3(texCoord, texIndex));
	#endif
}

