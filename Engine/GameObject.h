#pragma once
#include "Model.h"
#include "Rigidbody.h"
class ResourceManager;
class GameObject
{
public:
	Model* model;
	RigidBody* rb;
	std::string name;
	bool useRigidBodyCollider = true;
	bool drawCollisionModel = false;

	GameObject(Model* mod, std::string _name);

	void Update(float deltaTime);
	void Draw(Shader shader);
	void UpdateRigidBodyPositions();
};