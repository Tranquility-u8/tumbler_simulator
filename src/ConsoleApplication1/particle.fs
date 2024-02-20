#version 330 

in float Size;
in vec3 pos;
in float Alpha;
in float Age;
in float Life;
out vec4 color;
uniform sampler2D flameSpark;
uniform sampler2D flameStart;

void main()
{
    vec4 texColor;
    if((Age / Life) < 0.4)
        texColor = texture(flameSpark,gl_PointCoord);
    else 
        texColor = texture(flameStart,gl_PointCoord);
	texColor.a = texColor.r;
    color = vec4(0.5, 0.3, 0.1, Alpha) * texColor;

}
