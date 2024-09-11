#include "CollisionHandler.h"

CollisionDetails CalculateDetails(glm::vec3 axis, glm::vec2 projection1, glm::vec2 projection2, CollisionDetails* currentDetails, std::string name, int num)
{
	float longSpan = std::max(projection1.y, projection2.y) - std::min(projection1.x, projection2.x);
	float sumSpan = projection1.y - projection1.x + projection2.y - projection2.x;

	if (longSpan > sumSpan) //this means that there is no overlap between the two objects on this projection
	{
		currentDetails->overlapped = false;
		return *currentDetails;
	}

	float axisDepth = std::min(projection2.y - projection1.x, projection1.y - projection2.x);

	if (axisDepth < currentDetails->depth) //If this is more shallow than the other projections then use this one
	{
		float previousDepth = currentDetails->depth;
		glm::vec3 previousAxis = currentDetails->normal;
		currentDetails->depth = axisDepth;
		currentDetails->normal = axis;
		currentDetails->collisionType = name;
		currentDetails->number = num;
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

	Model* modelsToTest[2] = {
		&objects[0]->rb->collisionModel,
		&objects[1]->rb->collisionModel
	};
	CollisionDetails collisionDeets;
	//set default variables
	collisionDeets.depth = 999.f;
	collisionDeets.normal = glm::vec3(0);
	collisionDeets.overlapped = true; //It needs to prove that its not overlapping so we presume it is
	//If every projection fails we stay overlapped

	glm::vec3 objectScale[2] = { modelsToTest[0]->meshes[0].scale, modelsToTest[1]->meshes[0].scale};

	glm::vec3 objectPos[2] = { modelsToTest[0]->position, modelsToTest[1]->position };
	int objectMeshToTest[2] = { 0 };
	float objectClosestMeshDist[2]{ glm::distance(modelsToTest[0]->meshes[0].localSpacePosition + objectPos[0], objectPos[1]),
		glm::distance(modelsToTest[1]->meshes[0].localSpacePosition + objectPos[1], objectPos[0]) };
	//For loop so each test only needs to be written once, makes the code more condensed
	for (int o = 0; o < 2; o++)
	{
		for (int i = 1; i < modelsToTest[o]->meshes.size(); i++) //This is finding the closest child mesh so that we can test that.
		{//Useless in current system as we only have one mesh that is tested (the generated collision mesh)
			int otherObj = o == 1 ? 0 : 1;
			float dist = glm::distance(objectPos[o], objectPos[otherObj]);
			if (dist < objectClosestMeshDist[o])
			{
				objectClosestMeshDist[o] = dist;
				objectMeshToTest[o] = i;
				//cout << i << endl;
			}
		}
	}
	//Grab all the normals and edges of both objects (these are generated when the model is)
	vector<glm::vec3> normalsToTest[2] = { modelsToTest[0]->meshes[objectMeshToTest[0]].surfaceNormals, modelsToTest[1]->meshes[objectMeshToTest[1]].surfaceNormals };
	vector<glm::vec3> objectEdges[2] = { modelsToTest[0]->meshes[objectMeshToTest[0]].edges, modelsToTest[1]->meshes[objectMeshToTest[1]].edges };

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

			collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "normals " + objects[o]->name, i);
			if (!collisionDeets.overlapped)
			{
				break;
			}
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

			collisionDeets = CalculateDetails(axis, project1, project2, &collisionDeets, "edges " + objects[o]->name, i);
			if (!collisionDeets.overlapped)
			{
				break;
			}
		}

	}

	if (!collisionDeets.overlapped) return collisionDeets;

	//If we reached this point the objects overlap so return details

	float minDepth = 0.0001;
	collisionDeets.depth /= collisionDeets.normal.length();

	collisionDeets.depth;
	if (collisionDeets.depth < minDepth) collisionDeets.depth = minDepth;
	if ((collisionDeets.normal.x == 0 || collisionDeets.normal.x == -0) && (collisionDeets.normal.y == 0 || collisionDeets.normal.y == -0) &&
		(collisionDeets.normal.z == 0 || collisionDeets.normal.z == -0))
	{
		std::cout << "Normal not Valid" << std::endl;
		//Make sure the normal isnt 0 or else the objects will disappear
		collisionDeets.normal = glm::vec3(.01f, .01f, .01f);
		return collisionDeets;
	}
	else
	{
		collisionDeets.normal = glm::normalize(collisionDeets.normal);
	}
	//Make sure each collision normal is the correct direction or else the resolution could flip due to multiple tests
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
	//Loop through all vertices and project them along the axis we are testing, this is then used at the top function to determine if a collision occured
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