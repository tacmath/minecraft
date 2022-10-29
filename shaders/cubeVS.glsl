#version 330 core
layout (location = 0) in uint vertex;

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	uint y = vertex & 0xFFu;
	uint x = (vertex >> 8u) & 0xFu;
	uint z = (vertex >> 12u) & 0xFu;

   gl_Position = projection * view * vec4(x, y, z, 1.0f);
   texCoord = vec2((vertex >> 16u) & 1u, (vertex >> 17u) & 1u);
}