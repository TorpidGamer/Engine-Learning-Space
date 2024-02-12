#include "GameObject.h"
#include "ResourceManager.h"

GameObject::GameObject(Model* mod, std::string _name)
{
	model = mod;
	name = _name;
	rb = RigidBody();
	rb.collisionModel = rb.GenerateCollisionShape(model, 1, 1, 1);
	rb.position = model->position;
}

void GameObject::Update(float deltaTime)
{
	//rb.Step(deltaTime);
}

void GameObject::Draw(Shader shader)
{
	model->position = rb.position;
	rb.collisionModel.Draw(shader);
	model->Draw(shader);
}
