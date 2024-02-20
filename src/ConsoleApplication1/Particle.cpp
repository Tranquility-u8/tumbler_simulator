#include "particle.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SIZE 512

ParticleSystem::ParticleSystem(
	Shader& feedbackShader, 
	Shader& particleShader, 
	int texture1, 
	int texture2,
	float alpha,
	float cycleTime,
	float min_life,
	float max_life,
	float eccentricity,
	int init_nr_particles,
	int max_nr_particles

	){

	srand(time(NULL));

	this->texture1 = texture1;
	this->texture2 = texture2;
	this->alpha = alpha;
	this->cycleTime = cycleTime;
	this->min_life = min_life;
	this->max_life = max_life;
	this->eccentricity = eccentricity;

	
	this->init_nr_particles = init_nr_particles;
	this->max_nr_particles = max_nr_particles;

	this->mCurVBOIndex = 0;
	this->mCurTransformFeedbackIndex = 1;

	this->age = 0.0;
	this->isFirst = true;

	// init particleShader
	glm::vec3* randomSeries = new glm::vec3[SIZE];
	for (int i = 0; i < SIZE; i++)
	{
		randomSeries[i].x = float(rand()) / float(RAND_MAX);
		randomSeries[i].y = float(rand()) / float(RAND_MAX);
		randomSeries[i].z = float(rand()) / float(RAND_MAX);
	}
	
	glGenTextures(1, &this->_1dTexture);
	glBindTexture(GL_TEXTURE_1D, _1dTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, SIZE, 0, GL_RGB, GL_FLOAT, randomSeries);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	delete[] randomSeries;

	particleShader.setInt("flameSpark", 0);
	particleShader.setInt("flameStart", 1);

	Particle particles[10000];
	memset(particles, 0, sizeof(particles));

	// create launcher
	particles[0].type = PartcleType::launcher;
	particles[0].position = glm::vec3(0.0f);
	particles[0].age = 0.0f;
	particles[0].velocity = glm::vec3(0.0f, 0.1f, 0.0f);
	
	initSystem(particles, init_nr_particles);
	
	glGenTransformFeedbacks(2, this->transformFeedbacks);
	glGenBuffers(2, this->particleBuffers);
	glGenVertexArrays(2, this->particleArrays);
	
	for (int i = 0; i < 2; i++){
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->transformFeedbacks[i]);
		glBindBuffer(GL_ARRAY_BUFFER, this->particleBuffers[i]);
		glBindVertexArray(this->particleArrays[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->particleBuffers[i]);
	}
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glBindVertexArray(0);

	// init feedbackShader
	feedbackShader.use();
	glBindTexture(GL_TEXTURE_1D, _1dTexture);
	feedbackShader.setInt("random", 0);
}


void ParticleSystem::renderSystem(Shader& feedbackShader, Shader& particleShader,
	float time, const glm::mat4& model, const glm::mat4 view, const glm::mat4& projection)
{
	this->age += time * 1000.0f;
	updateSystem(feedbackShader, time * 1000.0f);
	renderParticle(particleShader, model, view, projection);
	
	// reverse buffer
	mCurVBOIndex = mCurTransformFeedbackIndex;
	mCurTransformFeedbackIndex = (mCurTransformFeedbackIndex + 1) & 0x1;
}

void ParticleSystem::updateSystem(Shader& feedbackShader, float time)
{
	// pass parameters to feedbackShader
	feedbackShader.use();
	feedbackShader.setFloat("a", 5);
	feedbackShader.setFloat("radius", 0.15);
	feedbackShader.setFloat("deltaTime", time);
	feedbackShader.setFloat("cycleTime", this->cycleTime);
	feedbackShader.setVec3("MAX_V", MAX_V);
	feedbackShader.setVec3("MIN_V", MIN_V);
	feedbackShader.setFloat("MAX_LIFE", this->max_life);
	feedbackShader.setFloat("MIN_LIFE", this->min_life);
	feedbackShader.setFloat("eccentricity", this->eccentricity);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, _1dTexture);
	feedbackShader.setInt("gRandomTexture",0);

	glEnable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(this->particleArrays[this->mCurVBOIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, this->particleBuffers[this->mCurVBOIndex]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->transformFeedbacks[this->mCurTransformFeedbackIndex]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, type));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, age));
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, alpha));
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
	glBeginTransformFeedback(GL_POINTS);
	
	// draw point sprite
	if (isFirst){
		glDrawArrays(GL_POINTS, 0, init_nr_particles);
		isFirst = false;
	} else {
		glad_glDrawTransformFeedback(GL_POINTS, this->transformFeedbacks[this->mCurVBOIndex]);
	}
	glEndTransformFeedback();
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);

	// recovery
	glDisable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::renderParticle(Shader& particleShader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection){

	glEnable(GL_POINT_SPRITE_COORD_ORIGIN);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	particleShader.use();
	particleShader.setMat4("model", model);
	particleShader.setMat4("view", view);
	particleShader.setMat4("projection", projection);

	glBindVertexArray(this->particleArrays[this->mCurTransformFeedbackIndex]);
	// bind transform feedback buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->particleBuffers[this->mCurTransformFeedbackIndex]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, alpha));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, age));
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));

	// set textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	// draw transform feedback
	glDrawTransformFeedback(GL_POINTS, this->transformFeedbacks[this->mCurTransformFeedbackIndex]);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void ParticleSystem::initSystem(Particle particles[], int nums){

	srand(time(NULL));

	int n = 10;
	float radius = 0.05f;
	for (int x = 0; x < nums; x++) {
		
		glm::vec3 p(0.0f);
		
		for (int y = 0; y < n; y++) {
			p.x += (2.0f * float(rand()) / float(RAND_MAX) - 1.0f);
			p.z += (2.0f * float(rand()) / float(RAND_MAX) - 1.0f);
		}
		
		p.x *= radius;
		p.z *= radius;
		p.y = center.y;
		
		particles[x].type = PartcleType::launcher;
		particles[x].alpha = this->alpha;
		particles[x].size = this->point_size;
		particles[x].age = (this->max_life - this->min_life) * (float(rand()) / float(RAND_MAX)) + this->min_life;
		particles[x].position = p;
		particles[x].velocity = MIN_V * (float(rand()) / float(RAND_MAX)) + MIN_V;
		
		float dist = sqrt(p.x * p.x + p.z * p.z);
		particles[x].lifetime = particles[x].age * (0.1 - dist);
	}
}
