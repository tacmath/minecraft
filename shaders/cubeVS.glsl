#version 400
layout (location = 0) in uint vertex;
layout (location = 1) in uint data;

out vec3 normal;
out vec3 texCoord;
out float luminosity;
out vec3	fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform vec2 chunkPos;

const vec3 cubeNormal[6] = vec3[6] (
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, -1.0f)
);

void main()
{
	uint y = vertex & 0xFFu;				// 8 bits
	uint x = (vertex >> 8u) & 0x1Fu;		// 5 bits
	uint z = (vertex >> 13u) & 0x1Fu;		// 5 bits

	texCoord.x = ((data >> 8u) & 1u); // test to use a padding between textures
	texCoord.y = ((data >> 12u) & 1u);
	texCoord.z = data & 0xFFu; // 8 bits

	luminosity = 1.0f - (((vertex >> 28u) & 0xFu) / 3.0f) * 0.8f;

	normal = cubeNormal[(data >> 16u) & 0xFu];

	fragPos = vec3(x + chunkPos.x, y, z + chunkPos.y);
	
	gl_Position = projection * view * vec4(fragPos, 1.0f);
}