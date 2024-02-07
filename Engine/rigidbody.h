#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "model.h"

struct BoxShape {
	float width, height;
	float mass;
	float momentOfInertia;

	void CalculateBoxIntertia(BoxShape* rb) {
		float m = rb->mass;
		float w = rb->width;
		float h = rb->height;
		rb->momentOfInertia = m * (w * w + h * h) / 12;
	}
};


class RigidBody {
public:
	glm::vec3 position;
	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 force;
	float mass = 1;
	bool isStatic = false;

	void Step(float dt, glm::vec3 forceToApply);
};