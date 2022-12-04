#version 400
out vec4 FragColor;

in vec3	normal;
in vec3 texCoord;
in float luminosity;
in vec4	fragPosLightSpace;

uniform vec3 lightDir;
uniform sampler2DArray atlas;
uniform sampler2D shadowMap;


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    if(projCoords.z > 1.0f)
        return 0.0f;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    float test = dot(normal, -lightDir);        // if the angle of the light and normal = ~ 90° set the shadow to 1
    if (test > -0.1f && test < 0.9f)
        return 1.0f;

    float bias = 0.0f;//mix(0.0005f, 0.000005f, (1.0f - dot(normal, lightDir)));

    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;        
        }    
    }
    shadow /= 9.0f;

    return shadow;
}  
/*
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    if(projCoords.z > 1.0f)
        return 0.0f;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float bias = max(0.0008f * (1.0f - dot(normal, lightDir)), 0.00005f);

    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    return shadow;
}  */

void main()
{
    float shadow = 1.0f - ShadowCalculation(fragPosLightSpace) * 0.5f;
    FragColor = vec4(vec3(texture(atlas, texCoord)) * luminosity * shadow, 1.0f);
}