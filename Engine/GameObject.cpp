#include "GameObject.h"
#include "ResourceManager.h"

GameObject::GameObject(Model* mod)
{
	model = mod;
	rb = RigidBody();
	rb.position = model->position;
}

void GameObject::Update(float deltaTime)
{
	//rb.Step(deltaTime);
}

void GameObject::Draw(Shader shader)
{
	model->position = rb.position;
	model->Draw(shader);
}
