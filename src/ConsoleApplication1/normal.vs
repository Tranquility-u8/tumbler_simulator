#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out VS_OUT {
    out vec2 vTexCoord;
    out vec3 vNormal;
    out vec3 vPos;
} vs_out;


void main()
{

    vs_out.vTexCoord = aTexCoords;
    vs_out.vNormal = aNormal;
    vs_out.vPos = aPos;
    gl_Position = vec4(aPos, 1.0);
}