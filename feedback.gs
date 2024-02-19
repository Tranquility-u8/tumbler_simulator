#version 330 core

layout (points) in;
layout (points, max_vertices = 10) out;

in float type0[];
in float age0[];
in float alpha0[];
in float size0[];
in float life0[];
in vec3 position0[];
in vec3 velocity0[];

out float type1;
out float age1;
out float alpha1;
out float size1;
out float life1;
out vec3 position1;
out vec3 velocity1;

uniform sampler1D gRandomTexture;

uniform float a;
uniform float deltaTime;
uniform float cycleTime;
uniform float eccentricity;
uniform float MAX_LIFE;
uniform float MIN_LIFE;

uniform vec3 MAX_V;
uniform vec3 MIN_V;

uniform float radius;

#define LAUNCHER 0.0
#define ROCKET 1.0

vec3 rand(float TexCoord);
vec3 randomDir(float TexCoord);

void main(){

    float age = age0[0] - deltaTime;

    // laucher:
	if(type0[0] == LAUNCHER){
        
        // time to launch a new particle
        if(age <= 0){

            type1 = ROCKET;
            position1 = position0[0];
            velocity1 = (MAX_V - MIN_V) * rand(age0[0]).x + MIN_V;
            age1 = (MAX_LIFE - MIN_LIFE) * rand(age0[0]).y + MIN_LIFE;
            float dist = sqrt(position1.x * position1.x + position1.z * position1.z);
		    age1 *= (1 + radius/(dist + 1));
            life1 = age1;
            alpha1 = alpha0[0];
            size1 = size0[0];
            
            EmitVertex();
            EndPrimitive();
            
            // reset age
            age = ((MAX_LIFE - MIN_LIFE) * rand(age0[0]).z + MIN_LIFE) * cycleTime;
        }
        
        // no need to change
        type1 = LAUNCHER;
        position1 = position0[0];
        velocity1 = velocity0[0];
        age1 = age;
        alpha1 = alpha0[0];
        size1 = size0[0];
        life1 = life0[0];
        
        EmitVertex();
        EndPrimitive();
    
    } 
    else{
    // rocket:
        if(age >= 0){       
            type1 = ROCKET;
            float time_secs = deltaTime / 1000.0;       
            vec3 offset = normalize(vec3(position0[0].x, 0, position0[0].z));
            position1 = position0[0] + velocity0[0] * time_secs + time_secs * offset * eccentricity;
            velocity1 = (velocity0[0] + time_secs  + time_secs * time_secs * a) * vec3(0.0,1.0,0.0);
            age1 = age;
            life1 = life0[0];
            float factor = 1.0 / ((age / 1000.0 - life1 / 2000.0) * (age / 1000.0 - life1 / 2000.0) + 1);
            alpha1 = factor / 5.0;
            size1 = 55.0 * factor;

            EmitVertex();
            EndPrimitive();
        }
    }
}

vec3 rand(float TexCoord){
    return texture(gRandomTexture,TexCoord).xyz; 
}

vec3 randomDir(float TexCoord){
	vec3 Dir = texture(gRandomTexture,TexCoord).xyz;
	Dir -= vec3(0.5);
	return Dir;
}
