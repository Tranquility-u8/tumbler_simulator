#pragma once
class Ashe {
public:
	float time;
	float cycleTime;
	float eccentricity;
	glm::vec3 position;
	ParticleSystem sparks;

	Ashe() {

	}
	Ashe(
		Shader& feedbackShader,
		Shader& particleShader,
		unsigned int texture1,
		unsigned int texture2,
		glm::vec3 position,
		float cycleTime,
		float eccentricity) {

		this->position = position;
		this->cycleTime = cycleTime;
		this->eccentricity = eccentricity;
		sparks = ParticleSystem(feedbackShader, particleShader, texture1, texture2, 0.0, this->cycleTime, 0.0f, 800.0f, this->eccentricity);
	}
	void render(Shader& feedbackShader, Shader& particleShader, float deltatime, glm::mat4 view, glm::mat4 projection) {
		glm::mat4 sparksModel = glm::rotate(glm::translate(glm::mat4(1.0f), this->position), glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
		this->sparks.renderSystem(feedbackShader, particleShader, deltatime, sparksModel, view, projection);
	}
};


