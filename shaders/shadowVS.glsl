#version 460
layout (location = 0) in uint vertex;
layout (location = 1) in uint data;

out vec3 texCoord;

uniform vec2 chunkPos;
uniform mat4 MVP;

void main()
{
	uint y = vertex & 0xFFu;				// 8 bits
	uint x = (vertex >> 8u) & 0x1Fu;		// 5 bits
	uint z = (vertex >> 13u) & 0x1Fu;		// 5 bits

	texCoord.x = ((data >> 8u) & 1u); // test to use a padding between textures
	texCoord.y = ((data >> 12u) & 1u);
	texCoord.z = data & 0xFFu; // 8 bits

	gl_Position = MVP * vec4(x + chunkPos.x, y, z + chunkPos.y, 1.0f);
}