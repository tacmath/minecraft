#version 460

#define SAMPLER_SHADOW

out vec4 FragColor;

in vec3	normal;
in vec3 texCoord;
in vec3 fragPos;
in float luminosity;

uniform float dayLightLevel;
uniform float timeCycle;
uniform mat4 view;
uniform vec3 lightDir;
uniform sampler2DArray atlas;

#ifdef SHADOW

const vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( 0.94558609, -0.76890725 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.94201624, -0.39906216 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

uniform mat4 lightSpaceMatrices[4];

#ifdef SAMPLER_SHADOW
uniform sampler2DArrayShadow shadowMap;
#else
uniform sampler2DArray shadowMap;
#endif

const int cascadeNB = 3;

const float cascadePlaneDistances[3] = float[3] (
	16.0f,
	48.0f,
	160.0f
);

const float cascadeNormalBias[3] = float[3] (
	0.05f,
	0.15f,
	0.25f
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
    float dotNL = dot(normal, -lightDir);
    if (dotNL > 0.0f) // if the angle of the light and normal = ~ 90� set the shadow to 1
        return 1.0f;

    vec4 fragPosViewSpace = view * vec4(fragPos, 1.0);
    int shadowMapLayer = getShadowLayer(abs(fragPosViewSpace.z)); 
    vec4 fragPosLightSpace = lightSpaceMatrices[shadowMapLayer] * vec4(fragPos + normal * cascadeNormalBias[shadowMapLayer], 1.0f);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if(projCoords.z > 1.0f)
        return 0.0f;

     // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float shadow = 0.0f;
    vec2 texelSize = (1.0f / vec2(textureSize(shadowMap, 0))) * 2.0f;
    for(int n = 0; n < 4; n++)
    {
            #ifdef SAMPLER_SHADOW
                shadow += texture(shadowMap, vec4(projCoords.xy + poissonDisk[n] * texelSize, shadowMapLayer, projCoords.z));
            #else
                float pcfDepth = texture(shadowMap, vec3(projCoords.xy + poissonDisk[n] * texelSize, shadowMapLayer)).r; 
                shadow += currentDepth > pcfDepth ? 1.0f : 0.0f;
            #endif

    }

    if (shadow == 0.0f || shadow == 4.0f)
       return shadow * 0.25f;

    for(int n = 4; n < 12; n++)
    {
            #ifdef SAMPLER_SHADOW
                shadow += texture(shadowMap, vec4(projCoords.xy + poissonDisk[n] * texelSize, shadowMapLayer, projCoords.z));
            #else
                float pcfDepth = texture(shadowMap, vec3(projCoords.xy + poissonDisk[n] * texelSize, shadowMapLayer)).r; 
                shadow += currentDepth > pcfDepth ? 1.0f : 0.0f;
            #endif

    }

    return shadow / 12;
}
#endif

void main()
{
    float light = dayLightLevel * luminosity;

    #ifdef SHADOW
    light *= (1.0f - ShadowCalculation() * 0.5f * timeCycle);
    #endif
    FragColor = vec4(vec3(texture(atlas, texCoord)) * light, 1.0f);
}