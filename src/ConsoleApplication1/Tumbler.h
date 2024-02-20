#pragma once
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Particle.h"
#include "Ashe.h"

#define NR_TUMBLER_WALL 6

class Ashe;

class Tumbler{

	friend class Ball;
public:
	enum TumblerState {
		Still = 0,
		Moving,
	};
	
	TumblerState state;
	glm::vec3 position;
	
	glm::vec3 swingDir;
	glm::mat4 rotateMatrix;
	
	float a; // amplitude
	float w; // frequency
	float t; 
	float theta;

	glm::vec3 box_min;
	glm::vec3 box_max;
	float wallPos[NR_TUMBLER_WALL];

	bool isBurned = false;
	bool isPrepare = false;

	Ashe ashe;

	Tumbler() {
		this->state = TumblerState::Still;
		this->position = glm::vec3(0.0, 0.0, 0.0);
		this->swingDir = glm::vec3(0.0, 0.0, 1.0);
		this->rotateMatrix = glm::mat4(1.0);
		this->a = 45.0;
		this->w = 0.15;
		this->t = 0.0;
	}
	
	Tumbler(glm::vec3 pos, glm::vec3 min, glm::vec3 max) {
		this->state = TumblerState::Still;
		this->position = pos;
		this->swingDir = glm::vec3(0.0, 0.0, 1.0);
		this->rotateMatrix = glm::mat4(1.0);
		this->a = 45.0;
		this->w = 0.15;
		this->t = 0.0;
		this->box_min = glm::vec3(5, 5, 5) * min;
		this->box_max = glm::vec3(5, 5, 5) * max;
	}
	
	void push(const glm::vec3 dir, const float force) {
		if (this->state == TumblerState::Still) {
			this->t = 0;
			this->a = 45.0 * (force + 30.0) / 30.0;
			this->swingDir = dir;
			state = TumblerState::Moving;
		}
		else {
			this->t = 0;
			this->swingDir = glm::normalize(this->swingDir + dir);
		}
	}


	void collision(const glm::vec3& ballPos, const float& speed) {
		glm::vec3 dis = ballPos - this->position;
		glm::vec3 dir = glm::normalize(glm::cross(glm::vec3(dis.x, 0.0, dis.z), glm::vec3(0.0, 1.0, 0.0)));
		if (this->state == TumblerState::Still) {
			this->t = 0;
			this->a = 45.0 * (speed + 30.0) / 30.0;
			this->swingDir = dir;
			state = TumblerState::Moving;
		}
		else {
			this->swingDir = glm::normalize(this->swingDir + dir);
		}
	}

	void update(float time) {
		if (state == TumblerState::Moving) {
			t += time * 25;
			float a_ = a * pow(2.7, -0.015 * t);
			if (a_ < 1.0) {
				state = TumblerState::Still;
				this->t = 0;
				rotateMatrix = glm::mat4(1.0);
				return;
			}
			theta = a_ * sin(w * t);
			glm::mat4 model = glm::mat4(1.0);
			rotateMatrix = glm::rotate(model, glm::radians(theta), this->swingDir);
		}
	}

	void updateBox() {

		// update aabbBox
		glm::vec3 box_max_ = this->box_min + this->position;
		glm::vec3 box_min_ = this->box_max + this->position;

		this->wallPos[0] = box_max_.x;
		this->wallPos[1] = box_min_.x;
		this->wallPos[2] = box_max_.y;
		this->wallPos[3] = box_min_.y;
		this->wallPos[4] = box_max_.z;
		this->wallPos[5] = box_min_.z;

	}

	void initBurned(
		Shader& feedbackShader,
		Shader& particleShader,
		unsigned int texture1,
		unsigned int texture2,
		float existTime,
		float eccentricity) {

		this->ashe = Ashe(feedbackShader, particleShader, texture1, texture2, this->position, existTime, eccentricity);
		this->position = glm::vec3(0.0, -10.0, 0.0);
		this->isPrepare = true;
	}
};

