#version 400
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2DArray depthMap;
uniform int index;

void main()
{             
    float depthValue = texture(depthMap, vec3(TexCoords, index)).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}  