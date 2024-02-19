#version 330 core 

layout (location = 0) in float type;
layout (location = 1) in vec3 position;
layout (location = 2) in vec3 velocity;
layout (location = 3) in float age;
layout (location = 4) in float alpha;
layout (location = 5) in float size;
layout (location = 6) in float life;

out float type0;
out float age0;
out float alpha0;
out float size0;
out float life0;
out vec3 position0;
out vec3 velocity0;

// we just use vs to pass data
void main()
{
	type0 = type;
	age0 = age;
    alpha0 = alpha;
    size0 = size;
    life0 = life;
	position0 = position;
	velocity0 = velocity;
}
