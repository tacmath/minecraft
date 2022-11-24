#version 400 core
layout (triangles) in;
layout (line_strip, max_vertices = 3) out;
//layout (triangle_strip, max_vertices = 3) out;

in vec3 texCoord[];
in float luminosity[];

out vec3 GtexCoord;
out float Gluminosity;
  
void main() {
    gl_Position = gl_in[0].gl_Position;
    GtexCoord = texCoord[0];
    Gluminosity = luminosity[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
     GtexCoord = texCoord[1];
    Gluminosity = luminosity[1];
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
     GtexCoord = texCoord[2];
    Gluminosity = luminosity[2];
    EmitVertex();
    
    EndPrimitive();
}  