#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; 

void main()
{
    for(int i = 0; i < 6; i++)
    {
        gl_Layer = i; 
        for(int j = 0; j < 3; j++) // for each triangle's vertices
        {
            FragPos = gl_in[j].gl_Position;
            gl_Position = shadowMatrices[i] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 