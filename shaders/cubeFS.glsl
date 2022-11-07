#version 400
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D atlas;

/*// for a fog effect
const float near = 0.1f;
const float far = 1000.0f;

float linearizeDepth(float depth) {
	return (2.0f * near * far) / (far + near - (depth * 2.0f - 1.0f) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

float depth = logisticDepth(gl_FragCoord.z, 0.3f, 16.0f * 19.0f);
FragColor = texture(atlas, texCoord) * (1.0f - depth) + vec4(depth * 1.0f);
*/

void main()
{
  //  FragColor = texelFetch(dirt, ivec2(texCoord * 16), 0);
    FragColor = texture(atlas, texCoord);
}