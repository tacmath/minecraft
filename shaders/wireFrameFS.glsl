#version 400
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoAo;

in vec3	Gnormal;
in vec3 GtexCoord;
in vec3 GfragPos;
in float Gluminosity;

uniform sampler2DArray atlas;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = GfragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = Gnormal;
    // and the diffuse per-fragment color
    gAlbedoAo.rgb = texture(atlas, GtexCoord).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoAo.a = Gluminosity;
}  