#pragma once
#include <string>
#include <map>
#include "ResourceManager.h"
#include "GameObject.h"

class Scene
{
public:
	std::map<std::string, GameObject*> SceneObjects;
	std::vector<RigidBody*> rbs;

public:
	void DrawScene(Shader shader);
	void UpdateScene(float deltaTime);
	void GetRigidBodies();
	glm::vec3 Orbit(RigidBody* rb1);
	virtual Scene* ConstructScene(ResourceManager rm);
};