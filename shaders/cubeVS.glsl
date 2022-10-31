#version 400
layout (location = 0) in uint vertex;

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform vec2 chunkPos;

void main()
{
	uint y = vertex & 0xFFu;				// 8 bits
	uint x = (vertex >> 8u) & 0x1Fu;		// 5 bits
	uint z = (vertex >> 13u) & 0x1Fu;		// 5 bits
	uint texAtlasX = (vertex >> 20u) & 0xFu;// 4 bits

	texCoord.x = (((vertex >> 18u) & 1u) + texAtlasX) / 3.0f;
	texCoord.y = ((vertex >> 19u) & 1u);


   gl_Position = projection * view * vec4(x + chunkPos.x, y, z + chunkPos.y, 1.0f);
}