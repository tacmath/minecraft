#version 400

out vec4 FragColor;

in vec3	normal;
in vec3 texCoord;
in vec3 fragPos;
in float luminosity;

uniform mat4 view;
uniform vec3 lightDir;
uniform sampler2DArray atlas;

#ifdef SHADOW
uniform mat4 lightSpaceMatrices[4];
uniform sampler2DArray shadowMap;

const int cascadeNB = 3;

const float cascadePlaneDistances[3] = float[3] (
	16.0f,
	48.0f,
	160.0f
);


int getShadowLayer(float depthValue) {
    
	int layer = cascadeNB - 1;
	for (int i = 0; i < cascadeNB; ++i) {
		if (depthValue < cascadePlaneDistances[i]) {
			layer = i;
			break;
		}
	}
	return layer;
}

float ShadowCalculation()
{
    vec4 fragPosViewSpace = view * vec4(fragPos, 1.0);
    int shadowMapLayer = getShadowLayer(abs(fragPosViewSpace.z)); 
    vec4 fragPosLightSpace = lightSpaceMatrices[shadowMapLayer] * vec4(fragPos + normal * 0.05f, 1.0f);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if(projCoords.z > 1.0f)
        return 0.0f;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if (dot(normal, -lightDir) > 0.0f) // if the angle of the light and normal = ~ 90� set the shadow to 1
        return 1.0f;

    float shadow = 0.0f;
    vec2 texelSize = 1.0f / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, shadowMapLayer)).r; 
            shadow += currentDepth > pcfDepth ? 1.0f : 0.0f;        
        }    
    }
    shadow /= 9.0f;

    return shadow;
}
#endif

void main()
{
    float day = clamp((lightDir.y - 0.1f) * 3.0f, 0.0f, 1.0f);
    float shadow = 1.0f - (1.0f - day) * 0.7f;

    #ifdef SHADOW
    if (day > 0.1f)
        shadow -= ShadowCalculation() * 0.5f * day;
    #endif
    FragColor = vec4(vec3(texture(atlas, texCoord)) * luminosity * shadow, 1.0f);
}