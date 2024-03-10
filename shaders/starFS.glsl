#version 400
out vec4 color;

in float alpha;

void main()
{
    color = vec4(1.0f, 1.0f, 1.0f, alpha);
}