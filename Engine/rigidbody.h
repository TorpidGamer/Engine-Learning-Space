#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Model.h"

using std::vector;

struct BoxShape {
	float width;
	float height;
	float depth;

	float mass;
	float momentOfInertia;

	void CalculateBoxIntertia(BoxShape* rb) {
		float m = rb->mass;
		float w = rb->width;
		float h = rb->height;
		float d = rb->depth;
		rb->momentOfInertia = m * (w * w + h * h + d * d) / 12;
	}
};


class RigidBody {
public:
	//glm::vec3 position;
	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 force;
	BoxShape PhysicsBox;
	Model collisionModel;
	Model* transform;
	float mass = 1;
	float friction = .95f;
	bool isStatic = false;
	bool colliding = false;

	void Step(float dt, glm::vec3 forceToApply);
	Model GenerateCollisionShape(Model* generateAround, float width = 1, float height = 1, float depth = 1);
	void AddForce(glm::vec3 forceToAdd);
};