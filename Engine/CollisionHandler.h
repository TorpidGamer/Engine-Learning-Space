#pragma once
#include "GameObject.h"

struct CollisionDetails
{
	bool overlapped;
	glm::vec3 normal = glm::vec3(0);
	float depth = -1;
	std::string collisionType;
};

CollisionDetails CalculateDetails(glm::vec3 axis, glm::vec2 projection1, glm::vec2 projection2, CollisionDetails* currentDetails,
	std::string name = "null");

CollisionDetails IsOverlapped(GameObject* obj1, GameObject* obj2);

glm::vec2 ProjectVertices(Model* verticesToProject, glm::vec3 axis, int meshToTest);