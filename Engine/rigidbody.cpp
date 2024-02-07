#include "GameObject.h"

void RigidBody::Step(float dt, glm::vec3 forceToApply)
{
	if (!isStatic)
	{
		force += forceToApply;
		velocity += force * dt;

		position += (velocity * dt);

		force = glm::vec3(0);
	}
}
