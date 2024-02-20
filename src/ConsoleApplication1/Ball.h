#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <ctime>
#include <algorithm>

#include "Tumbler.h"
#include "Ashe.h"

#define NR_TUMBLER_WALL 6


const glm::vec3 down = glm::vec3(0.0, -1.0, 0.0);
const float G = 1.0;



class Tumbler;
class ParticleSystem;

class Ball{

public:
	enum BallColor {
		White = 0,
		Red,
		Green,
		Blue,
		Board, // board texture
		Fire,  // fireball
	} color;

	enum BallType {
		Normalball = 0,
		Fireball,
		Dropball,
		DeadBall,
	} type;

	float speed;
	float radius;
	float lifetime; // for dropping ball

	Ashe ashe;
	glm::vec3 position;
	glm::vec3 velocity;

	bool startDrop = false;

	// default
	Ball() {
		this->color = BallColor::White;
		this->type = BallType::Normalball;
		this->lifetime = 0;
		this->radius = 0.12;
		this->position = glm::vec3(0.0);
		this->speed = 1.0;
		
		float x = rand() % 4 + 1;
		float y = rand() % 4 + 1;
		float z = rand() % 4 + 1;
		
		float sx = rand() % 2 * 2 - 1;
		float sy = rand() % 2 * 2 - 1;
		float sz = rand() % 2 * 2 - 1;
		this->velocity = glm::vec3(x * sx, y * sy, z * sz);
	}

	// normal
	Ball(glm::vec3 p) {
		this->color = BallColor::White;
		this->type = BallType::Normalball;
		this->lifetime = 0;
		this->radius = 0.12;
		this->position = p;
		this->speed = 1.0;
		
		float x = rand() % 4 + 1;
		float y = rand() % 4 + 1;
		float z = rand() % 4 + 1;
		
		float sx = rand() % 2 * 2 - 1;
		float sy = rand() % 2 * 2 - 1;
		float sz = rand() % 2 * 2 - 1;
		this->velocity = glm::vec3(x * sx, y * sy, z * sz);
	}

	// self-design
	Ball(glm::vec3 p, BallType t, glm::vec3 v) {
		if (t == BallType::Normalball)
			this->color = BallColor::White;
		else this->color = BallColor::Fire;
		this->type = t;
		this->lifetime = 0;
		this->radius = 0.12;
		this->position = p;
		this->speed = 1.0;
		this->velocity = v;
	}

	float getColor() const {
		return (float)color;
	}

	void update(float time) {
		if (this->type == BallType::Normalball || this->type == BallType::Fireball) {
			position += time * speed * velocity;
		}
		else if (this->type == BallType::Dropball){

			// update ball
			this->lifetime += time * 10;
			position += time * speed * glm::vec3(0.0, -1.0, 0.0);

			// become dead
			if (position.y < -2.5) 
				this->type = BallType::DeadBall;
		}

	}

	void drop() {
		this->type = BallType::Dropball;
		this->color = BallColor::Fire;
		this->startDrop = true;
		velocity = glm::vec3(0.0, -1.0, 0.0);
		std::cout << "a ball is burned!" << std::endl;
	}

	void initDrop(
		Shader& feedbackShader,
		Shader& particleShader,
		unsigned int texture1,
		unsigned int texture2,
		float existTime,
		float eccentricity) {

		this->startDrop = false;
		this->ashe = Ashe(feedbackShader, particleShader, texture1, texture2, this->position, existTime, eccentricity);
	}

	static bool checkCollisionBetweenBallAndWall(const Ball& ball, const glm::vec3& wallDir, float wallPos) {
		if (wallDir == glm::vec3(1.0, 0.0, 0.0)) {
			if (ball.position.x - wallPos <= ball.radius && ball.velocity.x < 0) return true;
		}
		else if (wallDir == glm::vec3(-1.0, 0.0, 0.0)) {
			if (wallPos - ball.position.x <= ball.radius && ball.velocity.x > 0) return true;
		}
		else if (wallDir == glm::vec3(0.0, 1.0, 0.0)) {
			if (ball.position.y - wallPos <= ball.radius && ball.velocity.y < 0) return true;
		}
		else if (wallDir == glm::vec3(0.0, -1.0, 0.0)) {
			if (wallPos - ball.position.y <= ball.radius && ball.velocity.y > 0) return true;
		}
		else if (wallDir == glm::vec3(0.0, 0.0, 1.0)) {
			if (ball.position.z - wallPos <= ball.radius && ball.velocity.z < 0) return true;
		}
		else if (wallDir == glm::vec3(0.0, 0.0, -1.0)) {
			if (wallPos - ball.position.z <= ball.radius && ball.velocity.z > 0) return true;
		} else return false;
		return false;
	}

	static void updateBetweenBallAndWall(Ball& ball, const glm::vec3& wallDir) {
		
		ball.velocity = glm::reflect(ball.velocity, wallDir);
		
		if (ball.type == BallType::Fireball) return;
		
		if (wallDir == glm::vec3(1.0, 0.0, 0.0)) {
			ball.color = BallColor::Red;
		}
		else if (wallDir == glm::vec3(-1.0, 0.0, 0.0)) {
			ball.color = BallColor::Blue;
		}
		else if (wallDir == glm::vec3(0.0, 1.0, 0.0)) {
			ball.color = BallColor::Board;
		}
		else if (wallDir == glm::vec3(0.0, -1.0, 0.0)) {
			ball.color = BallColor::White;
		}
		else if (wallDir == glm::vec3(0.0, 0.0, 1.0)) {
			ball.color = BallColor::Green;
		}
		else if (wallDir == glm::vec3(0.0, 0.0, -1.0)) {

		}
	}

	static bool checkCollisionBetweenBalls(const Ball& a, const Ball& b) {
		
		glm::vec3 d = a.position - b.position;
		if (glm::dot(a.velocity, d) > 0 && glm::dot(b.velocity, d) < 0)
			return false;
		
		if (glm::dot(d, d) <= (a.radius + b.radius) * (a.radius + b.radius))
			return true;

		return false;
	}

	static void updateBetweenBalls(Ball& a, Ball& b) {
		
		BallType ta = a.type;
		BallType tb = b.type;
		glm::vec3 n = glm::normalize(b.position - a.position);
		glm::vec3 u = glm::dot(a.velocity - b.velocity, n) * n;
		
		if (ta == BallType::Fireball && tb == BallType::Normalball) {
			a.velocity -= u;
			b.drop();
		}
		else if (tb == BallType::Fireball && ta == BallType::Normalball) {
			b.velocity += u;
			a.drop();
		}
		else {
			a.velocity -= u;
			b.velocity += u;
		}
		

	}

	static int checkCollisionBetweenBallAndTumbler(const Ball& ball, Tumbler& tumbler, const glm::vec3 wallDirs[]) {
		
		float nearestX = std::max(tumbler.wallPos[1], std::min(ball.position.x, tumbler.wallPos[0]));
		float nearestY = std::max(tumbler.wallPos[3], std::min(ball.position.y, tumbler.wallPos[2]));
		float nearestZ = std::max(tumbler.wallPos[5], std::min(ball.position.z, tumbler.wallPos[4]));

		float distX = ball.position.x - nearestX;
		float distY = ball.position.y - nearestY;
		float distZ = ball.position.z - nearestZ;

		float X2 = distX * distX;
		float Y2 = distY * distY;
		float Z2 = distZ * distZ;

		float distance = sqrt(X2 + Y2 + Z2);

		if(distance >= ball.radius) return -1;
		
		float d = std::max(X2, std::max(Y2, Z2));
		if (d == X2) {
			if (distX > 0) {
				if (ball.velocity.x < 0) {
					if (ball.type == BallType::Fireball) {
						tumbler.isBurned = true;
						cout << "a tumbler is burned!" << endl;
					}
					return 0;
				}
			}
			else {
				if (ball.velocity.x > 0) {
					if (ball.type == BallType::Fireball) {
						tumbler.isBurned = true;
						cout << "a tumbler is burned!" << endl;
					}
					return 1;
				}
			}
		}
		else if (d == Y2) {
			if (distY > 0) {
				if (ball.velocity.y < 0) {
					if (ball.type == BallType::Fireball) {
						tumbler.isBurned = true;
						cout << "a tumbler is burned!" << endl;
					}
					return 2;
				}
			}
			else {
				if (ball.velocity.y > 0) {
					if (ball.type == BallType::Fireball) {
						tumbler.isBurned = true;
						cout << "a tumbler is burned!" << endl;
					}
					return 3;
				}
			}
		} else {
			if (distZ > 0) {
				if (ball.velocity.z < 0) {
					if (ball.type == BallType::Fireball) {
						tumbler.isBurned = true;
						cout << "a tumbler is burned!" << endl;
					}
					return 4;
				}
			}
			else {
				if (ball.velocity.z > 0) {
					if (ball.type == BallType::Fireball) {
						tumbler.isBurned = true;
						cout << "a tumbler is burned!" << endl;
					}
					return 5;
				}
			}

		}
		return -1;

	}

	static void updateBetweenBallAndTumbler(Ball& ball, Tumbler& tumbler, const glm::vec3 wallDir) {
		
		//update ball
		if (ball.type == BallType::Normalball)
			ball.color = BallColor::Red;
		ball.velocity = glm::reflect(ball.velocity, wallDir);
		
		//update tumbler
		tumbler.collision(ball.position, ball.speed);
	}

};

