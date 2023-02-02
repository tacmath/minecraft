#version 400
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoAo;

in vec3	normal;
in vec3 texCoord;
in vec3 fragPos;
in float luminosity;

uniform sampler2DArray atlas;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normal;
    // and the diffuse per-fragment color
    gAlbedoAo.rgb = texture(atlas, texCoord).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoAo.a = luminosity;
}  