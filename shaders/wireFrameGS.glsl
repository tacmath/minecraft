#version 400 core
layout (triangles) in;
layout (line_strip, max_vertices = 3) out;

in vec3 texCoord[];
in float luminosity[];
  
void main() {
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}  