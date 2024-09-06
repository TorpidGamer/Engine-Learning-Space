#include "Scene.h"

void Scene::DrawScene(Shader shader)
{
	int l = 0;
	for (auto& i : SceneObjects)
	{
		l++;
		i.second->Draw(shader);
	}
	//std::cout << "there are " << l << " objects" << std::endl;
}

void Scene::UpdateScene(float deltaTime)
{	
	for (auto& i : SceneObjects)
	{
		i.second->rb->Step(deltaTime, Orbit(i.second->rb));//glm::vec3(0, -1, 0);
		i.second->Update(deltaTime);//rb.Step(deltaTime, /*Orbit(&i.second->rb)*/ glm::vec3(0, -1, 0));
	}
}

Scene* Scene::ConstructScene(ResourceManager rm)
{
	for (auto& i : SceneObjects)
	{
		i.second->rb->collisionModel = i.second->rb->GenerateCollisionShape(i.second->model);
		std::cout << "LOADED: " << i.first << std::endl;
	}
	return this;
}

void Scene::GetRigidBodies()
{
	for (auto& i : SceneObjects)
	{
		rbs.push_back(i.second->rb);
	}
}

glm::vec3 Scene::Orbit(RigidBody* rb1)
{
	glm::vec3 forceToReturn = glm::vec3(0);
	float G_const = 6.67408e-1f;
	for (auto y : SceneObjects)
	{
		if (rb1 == y.second->rb)
			continue;

		RigidBody* rb2 = y.second->rb;

		if (rb2 == nullptr)
		{
			std::cout << "Failed to get second rigidbody" << std::endl;
			continue;
		}

		if (rb1 == nullptr)
		{
			std::cout << "First rigidbody is a nullptr??" << std::endl;
			continue;
		}

		if (rb1->collisionModel.position.x != rb1->collisionModel.position.x)
			std::cout << "RB1 pos is nan" << std::endl;

		if (rb2->collisionModel.position.x != rb2->collisionModel.position.x)
			std::cout << "RB2 pos is nan" << std::endl;

		float distance = glm::pow((rb1->collisionModel.position.x - rb2->collisionModel.position.x), 2)
			+ glm::pow((rb1->collisionModel.position.y - rb2->collisionModel.position.y), 2) + glm::pow((rb1->collisionModel.position.z - rb2->collisionModel.position.z), 2);
		if (distance == 0)
		{
			distance = 0.00000001f;
		}
		if (distance < 0)
		{
			distance *= -1;
		}
		distance = glm::sqrt(distance);
		float acceleration = (G_const * rb2->mass) / glm::pow(distance, 3);
		float gravX = acceleration * (rb2->collisionModel.position.x - rb1->collisionModel.position.x);
		float gravY = acceleration * (rb2->collisionModel.position.y - rb1->collisionModel.position.y);
		float gravZ = acceleration * (rb2->collisionModel.position.z - rb1->collisionModel.position.z);

		//std::cout << "distance: " << distance << std::endl;
		//std::cout << "acceleration: " << acceleration << std::endl;
		//std::cout << "gravX: " << gravX << std::endl;
		//std::cout << "gravY: " << gravY << std::endl;
		//std::cout << "gravZ: " << gravZ << std::endl;
		//std::cout << std::endl;

		forceToReturn += glm::vec3(gravX, gravY, gravZ) *= G_const;
	}
	return forceToReturn;
}
