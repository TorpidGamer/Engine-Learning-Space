#pragma once
#include "model.h"
#include "rigidbody.h"
class ResourceManager;
class GameObject
{
public:
	Model* model;
	RigidBody rb;

	GameObject(Model* mod);

	void Update(float deltaTime);
	void Draw(Shader shader);
};