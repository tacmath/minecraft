#version 400 core
layout (triangles) in;
layout (line_strip, max_vertices = 3) out;
//layout (triangle_strip, max_vertices = 3) out;

in vec3 texCoord[];
in float luminosity[];
in vec3	normal[];
in vec3 fragPos[];

out vec3 Gnormal;
out vec3 GfragPos;
out vec3 GtexCoord;
out float Gluminosity;
  
void main() {
    for (int n = 0; n < 3; n++) {
        gl_Position = gl_in[n].gl_Position;
        Gnormal = normal[n];
        GfragPos = fragPos[n];
        GtexCoord = texCoord[n];
        Gluminosity = luminosity[n];
        EmitVertex();
    }
    
    EndPrimitive();
}  