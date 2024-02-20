#pragma once
#include <iostream>
#include <ctime>
#include <math.h>
#include <glm/glm.hpp>
#include "shader.h"

#define MAX_V glm::vec3(0.0,3.0,0.0)
#define MIN_V glm::vec3(0.0,1.0,0.0)


class ParticleSystem{

	class Particle {
		friend ParticleSystem;
	public:
		float type;
		float age;
		float alpha;
		float size;
		float lifetime;
		glm::vec3 position;
		glm::vec3 velocity;
	};

	enum PartcleType {
		launcher = 0,
		rocket
	};

public:
	ParticleSystem() {};
	ParticleSystem(
		Shader& feedbackShader,
		Shader& particleShader,
		int texture1,
		int texture2,
		float alpha = 0.2f,
		float cycleTime = 1.0,
		float min_life = 100.0f,
		float max_life = 600.0f,
		float eccentricity = 0.0,
		int init_nr_particles = 1500,
		int max_nr_particles = 4000
	);
	~ParticleSystem(){}
	void renderSystem(Shader& feedbackShader, Shader& particleShader, float time, const glm::mat4& model, const glm::mat4 view, const glm::mat4& projection);
	void updateSystem(Shader& feedbackShader, float time);

private:
	bool init(glm::vec3& pos);
	void initSystem(Particle partciles[], int nums);
	void renderParticle(Shader& particleShader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

	float age;
	bool isFirst;

	// texture
	int texture1;
	int texture2;
	GLuint _1dTexture;
	
	// transform feedback
	GLuint transformFeedbacks[2];
	unsigned int mCurVBOIndex, mCurTransformFeedbackIndex;
	GLuint particleBuffers[2];
	GLuint particleArrays[2];

	// parameters for system	
	glm::vec3 center = glm::vec3(0.0f);
	
	int init_nr_particles;
	int max_nr_particles;
	
	float cycleTime = -1.0; // -1 means infinite
	float min_life = 100.0f;
	float max_life = 500.0f;
	float alpha = 0.2f;
	float point_size = 30.0f;
	float eccentricity = 1.0f;

};
