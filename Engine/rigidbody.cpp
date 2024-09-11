#include "GameObject.h"

void RigidBody::Step(float dt, glm::vec3 forceToApply)
{
	if (!isStatic)
	{
		force += forceToApply;
		velocity += force * dt;
		if (colliding)	velocity *= friction; //If colliding apply friction
		//std::cout << "velocity: " << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl;

		transform->position += velocity;

		force = glm::vec3(0);
	}
	collisionModel.position = transform->position * transform->meshes[0].scale;
}

void RigidBody::AddForce(glm::vec3 forceToAdd)
{
	force += forceToAdd;
}

Model RigidBody::GenerateCollisionShape(Model* generateAround, float width, float height, float depth)
{
	transform = generateAround;
	float top = 0, right = 0, left = 0, bottom = 0, back = 0, front = 0;
	if (generateAround != nullptr)
	{
		//Loops through all meshes
		for (int j = 0; j < generateAround->meshes.size(); j++)
		{
			Mesh curMesh = generateAround->meshes[j];
			for (int i = 0; i < curMesh.vertices.size(); i++)
			{
				//Loops through each vertex to find the outer bounds of the model to wrap the shape around
				glm::vec3 curPoint = curMesh.vertices[i].vertexPosition * generateAround->meshes[j].scale;
				if (curPoint.y > top) top = curPoint.y;
				if (curPoint.y < bottom) bottom = curPoint.y;
				if (curPoint.x > right) right = curPoint.x;
				if (curPoint.x < left) left = curPoint.x;
				if (curPoint.z > front) front = curPoint.z;
				if (curPoint.z < back) back = curPoint.z;
			}
		}
	}
	else
	{
		std::cout << "Using predefined measurements to generate collision shape" << std::endl;
		top = height;
		bottom = -height;
		right = width;
		left = -width;
		front = depth;
		back = -depth;
	}
	//Generates a model for the collision shape
	vector<Vertex> vertices = {
		Vertex(glm::vec3(left, top, back), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec3(left, top, front), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec3(right, top, back), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec3(right, top, front), glm::vec3(0), glm::vec2(0)),

		Vertex(glm::vec3(left, bottom, back), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec3(left, bottom, front), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec3(right, bottom, back), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec3(right, bottom, front), glm::vec3(0), glm::vec2(0))
	};

	vector<unsigned int> indices = {
		//Top
		0, 2, 1,
		1, 2, 3,

		//Bottom
		4, 6, 5,
		5, 6, 7,

		//Right
		3, 2, 7,
		7, 2, 6,

		//Left
		1, 0, 5,
		5, 0, 4,

		//Back
		0, 2, 4,
		4, 2, 6,

		//Front
		1, 3, 5,
		5, 3, 7
	};
	return Model(Mesh(vertices, indices, vector<Texture>(), true));
}