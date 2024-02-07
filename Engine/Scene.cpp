#include "Scene.h"

void Scene::DrawScene(Shader shader)
{
	for (auto& i : SceneObjects)
	{
		i.second->Draw(shader);
	}
}

void Scene::UpdateScene(float deltaTime)
{	
	for (auto& i : SceneObjects)
	{
		i.second->rb.Step(deltaTime, Orbit(&i.second->rb));
	}
}

void Scene::GetRigidBodies()
{
	for (auto& i : SceneObjects)
	{
		rbs.push_back(&i.second->rb);
	}
}

glm::vec3 Scene::Orbit(RigidBody* rb1)
{
	glm::vec3 forceToReturn = glm::vec3(0);
	float G_const = 6.67408e-11;
	for (auto y : SceneObjects)
	{
		if (rb1 == &y.second->rb)
			continue;

		RigidBody* rb2 = &y.second->rb;

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

		if (rb1->position.x != rb1->position.x)
			std::cout << "RB1 pos is nan" << std::endl;

		if (rb2->position.x != rb2->position.x)
			std::cout << "RB2 pos is nan" << std::endl;

		float distance = glm::pow((rb1->position.x - rb2->position.x), 2) + glm::pow((rb1->position.y - rb2->position.y), 2) + glm::pow((rb1->position.z - rb2->position.z), 2);
		if (distance == 0)
		{
			distance = 0.00000001f;
		}
		if (distance < 0)
		{
			distance *= -1;
		}
		distance = glm::sqrt(distance);
		float acceleration = (rb2->mass) / distance;
		float gravX = acceleration * (rb2->position.x - rb1->position.x);
		float gravY = acceleration * (rb2->position.y - rb1->position.y);
		float gravZ = acceleration * (rb2->position.z - rb1->position.z);

		//std::cout << "distance: " << distance << std::endl;
		//std::cout << "acceleration: " << acceleration << std::endl;
		//std::cout << "gravX: " << gravX << std::endl;
		//std::cout << "gravY: " << gravY << std::endl;
		//std::cout << "gravZ: " << gravZ << std::endl;
		//std::cout << std::endl;

		forceToReturn += glm::vec3(gravX, gravY, gravZ);
	}
	return forceToReturn;
}
