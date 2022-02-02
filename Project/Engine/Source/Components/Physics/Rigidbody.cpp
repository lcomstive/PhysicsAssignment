#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace std;
using namespace Engine::Physics;
using namespace Engine::Components;

PhysicsSystem& Rigidbody::GetSystem() { return GetGameObject()->GetScene()->GetPhysics(); }

void Rigidbody::FixedUpdate(float timestep)
{
	Transform* transform = GetGameObject()->GetComponent<Transform>();

	// Apply gravity
	Force += Mass * GetSystem().GetGravity();

	Velocity += Force / Mass * timestep;
	transform->Position += Velocity * timestep;

	Force = { 0, 0, 0 }; // Reset applied force
}

void Rigidbody::ApplyForce(glm::vec3 force)
{
	Force += force;
}