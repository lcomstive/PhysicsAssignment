#pragma once
#include <glm/glm.hpp>
#include <Engine/Physics/Shapes.hpp>

namespace Engine::Components { struct Rigidbody; }

namespace Engine::Physics
{
	struct CollisionInfo
	{
		glm::vec3 A = { 0, 0, 0 };		// Furthest point of A into B
		glm::vec3 B = { 0, 0, 0 };		// Furthest point of B into A
		glm::vec3 Normal = { 0, 0, 0 };	// B - A normalised
		float Depth = 0;				// Length of B - A
		bool HasCollided = false;		// Has a collision occurred?
	};

	struct Collision
	{
		Engine::Components::Rigidbody* RigidbodyA;
		Engine::Components::Rigidbody* RigidbodyB;
		CollisionInfo Info;
	};

	Collision TestSphereBoxCollider(Sphere a, Engine::Components::Rigidbody* aRb, OBB b, Engine::Components::Rigidbody* bRb);
	Collision TestSpherePlaneCollider(Sphere a, Engine::Components::Rigidbody* aRb, Plane b, Engine::Components::Rigidbody* bRb);
	Collision TestSphereSphereCollider(Sphere a, Engine::Components::Rigidbody* aRb, Sphere b, Engine::Components::Rigidbody* bRb);

	Collision TestBoxBoxCollider(AABB a, Engine::Components::Rigidbody* aRb, OBB b, Engine::Components::Rigidbody* bRb);
	Collision TestBoxBoxCollider(OBB a, Engine::Components::Rigidbody* aRb, OBB b, Engine::Components::Rigidbody* bRb);
	Collision TestBoxPlaneCollider(OBB a, Engine::Components::Rigidbody* aRb, Plane b, Engine::Components::Rigidbody* bRb);
	
	Collision TestPlanePlaneCollider(Plane a, Engine::Components::Rigidbody* aRb, Plane b, Engine::Components::Rigidbody* bRb);
}