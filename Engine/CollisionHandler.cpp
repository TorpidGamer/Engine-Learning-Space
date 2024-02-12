#include "CollisionHandler.h"

CollisionDetails CalculateDetails(glm::vec3 axis, glm::vec2 projection1, glm::vec2 projection2, CollisionDetails* currentDetails, std::string name)
{
	float longSpan = std::max(projection1.y, projection2.y) - std::min(projection1.x, projection2.x);
	float sumSpan = projection1.y - projection1.x + projection2.y - projection2.x;

	if (longSpan > sumSpan)
	{
		currentDetails->overlapped = false;
		return *currentDetails;
	}

	float axisDepth = std::min(projection2.y - projection1.x, projection1.y - projection2.x);

	if (axisDepth < currentDetails->depth)
	{
		float previousDepth = currentDetails->depth;
		glm::vec3 previousAxis = currentDetails->normal;
		currentDetails->depth = axisDepth;
		currentDetails->normal = axis;
		currentDetails->collisionType = name;
		if (currentDetails->normal.x == 0 && currentDetails->normal.y == 0 && currentDetails->normal.z == 0)
		{
			currentDetails->depth = previousDepth;
			currentDetails->normal = previousAxis;
		}
		if (isnan(currentDetails->normal.x)) std::cout << " is nan" << std::endl;
	}
	return *currentDetails;
}

CollisionDetails IsOverlapped(GameObject* obj1, GameObject* obj2)
{
	GameObject* objects[2] = {
		obj1, obj2
	};
	//second point - first point = edge
	Model* modelsToTest[2] = {
		objects[0]->useRigidBodyCollider ? &objects[0]->rb.collisionModel : objects[0]->model,
		objects[1]->useRigidBodyCollider ? &objects[1]->rb.collisionModel : objects[1]->model
	};
	CollisionDetails collisionDeets;

	collisionDeets.depth = 999.f;
	collisionDeets.normal = glm::vec3(0);
	collisionDeets.overlapped = true;

	glm::vec3 objectScale[2] = { objects[0]->model->meshes[0].scale, objects[1]->model->meshes[0].scale };

	glm::vec3 objectPos[2] = { objects[0]->rb.position, objects[1]->rb.position };
	int objectMeshToTest[2] = { 0 };
	float objectClosestMeshDist[2]{ glm::distance(objects[0]->model->meshes[0].localSpacePosition + objectPos[0], objectPos[1]),
		glm::distance(objects[1]->model->meshes[0].localSpacePosition + objectPos[1], objectPos[0]) };

	for (int o = 0; o < 1; o++)
	{
		for (int i = 1; i < modelsToTest[o]->meshes.size(); i++)
		{
			int otherObj = o == 1 ? 0 : 1;
			float dist = glm::distance(modelsToTest[o]->meshes[i].localSpacePosition + objectPos[o], objectPos[otherObj]);
			if (dist < objectClosestMeshDist[o])
			{
				objectClosestMeshDist[o] = dist;
				objectMeshToTest[o] = i;
				//cout << i << endl;
			}
		}
	}
	//glm::vec3 objectMeshPosition[2] = { modelsToTest[0]->meshes[objectMeshToTest[0]].position, modelsToTest[1]->meshes[objectMeshToTest[1]].position };
	vector<glm::vec3> normalsToTest[2] = { modelsToTest[0]->meshes[objectMeshToTest[0]].surfaceNormals, modelsToTest[1]->meshes[objectMeshToTest[1]].surfaceNormals };
	vector<glm::vec3> objectEdges[2] = { modelsToTest[0]->meshes[objectMeshToTest[0]].edges, modelsToTest[1]->meshes[objectMeshToTest[1]].edges };
	//cout << "Testing obj1: " << obj1->name << ", mesh no. " << meshToTest1 << endl;
	//cout << "Testing obj2: " << obj2->name << ", mesh no. " << meshToTest2 << endl;

	for (int o = 0; o < 2; o++)
	{
		int otherObj = o == 1 ? 0 : 1;
		//Test normals
		for (int i = 0; i < normalsToTest[o].size(); i++)
		{
			glm::vec2 project1, project2;
			glm::vec3 axis = (normalsToTest[o][i]); //* -objects[o]->scale));// + objects[o]->position);
			project1 = ProjectVertices(modelsToTest[o], axis, objectMeshToTest[o]);
			project2 = ProjectVertices(modelsToTest[otherObj], axis, objectMeshToTest[otherObj]);

			collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "normals " + objects[o]->name);
		}

		//Test edges
		for (int i = 0; i < objectEdges[o].size(); i++)
		{
			if (objectEdges[otherObj].size() == 0)
			{
				std::cout << "No edges on Obj" << otherObj + 1 << std::endl;
				break;
			}
			glm::vec3 axis1 = (objectEdges[o][i]); //* -objects[o]->scale); //+ objects[o]->position;
			glm::vec3 axis2 = (objectEdges[otherObj][i % objectEdges[otherObj].size()]); //* -objects[otherObj]->scale);// + objects[otherObj]->position;
			glm::vec3 axis = glm::cross(axis1, axis2);

			glm::vec2 project1, project2;
			project1 = ProjectVertices(modelsToTest[o], axis, objectMeshToTest[o]);
			project2 = ProjectVertices(modelsToTest[otherObj], axis, objectMeshToTest[otherObj]);

			collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "edges " + objects[o]->name);
		}
	}

	//If we reached this point the objects overlap so return details

	float minDepth = 0.0001;
	collisionDeets.depth /= collisionDeets.normal.length();

	collisionDeets.depth;
	if (collisionDeets.depth < minDepth) collisionDeets.depth = minDepth;
	if (collisionDeets.normal.x == 0 && collisionDeets.normal.y == 0 && collisionDeets.normal.z == 0); //cout << "Normal not Valid" << endl;
	else
	{
		collisionDeets.normal = glm::normalize(collisionDeets.normal);
		collisionDeets.normal.x = 0;
		collisionDeets.normal.z = 0;
	}
	if (isnan(collisionDeets.normal.x)) std::cout << "Normalized Normal Nan" << std::endl;
	glm::vec3 intendedDir = modelsToTest[1]->position - modelsToTest[0]->position;
	if (glm::dot(intendedDir, collisionDeets.normal) <= 0.f)
	{
		collisionDeets.normal = -collisionDeets.normal;
	}
	collisionDeets.overlapped = true;
	return collisionDeets;
}

glm::vec2 ProjectVertices(Model* verticesToProject, glm::vec3 axis, int meshToTest)
{
	float min = 999;
	float max = -999;
	for (int i = 0; i < verticesToProject->meshes[meshToTest].vertices.size(); i++)
	{
		glm::vec3 currentVert = (verticesToProject->meshes[meshToTest].vertices[i].vertexPosition * verticesToProject->meshes[meshToTest].scale) + verticesToProject->position;
		if (isnan(currentVert.x) || isnan(currentVert.y) || isnan(currentVert.z))
		{
			std::cout << "error isNan on: " << std::endl;
			currentVert = glm::vec3(0);
		}
		float projection = glm::dot(currentVert, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
	return glm::vec2(min, max);
}