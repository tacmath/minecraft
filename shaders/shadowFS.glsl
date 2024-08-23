#version 460

in vec3 texCoord;
uniform sampler2DArray atlas;

void main()
{
    vec4 color = texture(atlas, texCoord);
    if (color.w < 0.5f)
        discard;
}