#version 400
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D sunTexture;

void main()
{
    vec4 color = texture(sunTexture, texCoord);
    if ( color.r + color.g + color.b < 0.8f) {
       discard;
    }
    FragColor = color;

}