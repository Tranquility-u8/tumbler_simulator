#version 330 core

#define G 1
#define V 12

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 vTexCoord;
    vec3 vNormal;
    vec3 vPos;
} gs_in[];

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform int isDrop;
uniform float time;

vec3 getNewNormal();
vec4 calculatePos(vec4 position, vec3 normal);

void main() {    
    
    vec3 normal = gs_in[0].vNormal;
    mat4 pv = projection * view;

    // emit vertex 
    vec4 p0 = model * calculatePos(gl_in[0].gl_Position, normal);
    FragPos = vec3(p0);
    gl_Position = pv * p0;
    TexCoord = gs_in[0].vTexCoord;
    Normal = normal;                     
    EmitVertex();
    
    vec4 p1 = model * calculatePos(gl_in[1].gl_Position, normal);
    FragPos = vec3(p1);
    gl_Position = pv * p1;
    TexCoord = gs_in[1].vTexCoord;
    Normal = normal;                     
    EmitVertex();
    
    vec4 p2 = model * calculatePos(gl_in[2].gl_Position, normal);
    FragPos = vec3(p2);
    gl_Position = pv * p2;
    TexCoord = gs_in[2].vTexCoord;
    Normal = normal;                     
    EmitVertex();

    EndPrimitive();
}

vec3 getNewNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 calculatePos(vec4 position, vec3 normal)
{
    if (isDrop == 1) {
        vec3 down = vec3(0.0, 1.0, 0.0);
        //float factor = dot(vec3(0.1), normal);
        vec3 direction = normal * time * V; 
        return position + vec4(direction, 1.0);
    } else 
        return position;
}
