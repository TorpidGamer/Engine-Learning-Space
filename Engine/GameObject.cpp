#include "GameObject.h"
#include "ResourceManager.h"

GameObject::GameObject(Model* mod, std::string _name)
{
	model = mod;
	name = _name;
	rb = new RigidBody();
	//rb->collisionModel = rb->GenerateCollisionShape(model);
	//rb->position = model->position;
	rb->collisionModel.position = model->position * model->meshes[0].scale;
}

void GameObject::Update(float deltaTime)
{
	//rb->Step(deltaTime, glm::vec3(0, 0.f, 0));
	//rb->collisionModel.position = rb->position;
}

void GameObject::Draw(Shader shader)
{
	//model->position = rb->position;
	if (drawCollisionModel)	rb->collisionModel.Draw(shader);
	model->Draw(shader);
}

void GameObject::UpdateRigidBodyPositions()
{
	//rb->position = model->position;
	//rb->collisionModel.meshes[0].scale = model->meshes[0].scale;
	rb->collisionModel.position = model->position * model->meshes[0].scale;
}

class Player : GameObject
{	

};
