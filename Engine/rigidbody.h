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
	glm::vec3 position;
	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 force;
	BoxShape PhysicsBox;
	Model collisionModel;
	float mass = 1;
	bool isStatic = false;

	void Step(float dt, glm::vec3 forceToApply);
	Model GenerateCollisionShape(Model* generateAround, float width = 0, float height = 0, float depth = 0);
};