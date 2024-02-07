#pragma once
#include <string>
#include <map>
#include "model.h"
#include "GameObject.h"

class Scene
{
public:
	std::map<std::string, GameObject*> SceneObjects;
	std::vector<RigidBody*> rbs;

	void DrawScene(Shader shader);
	void UpdateScene(float deltaTime);
	void GetRigidBodies();
	glm::vec3 Orbit(RigidBody* rb1);
};