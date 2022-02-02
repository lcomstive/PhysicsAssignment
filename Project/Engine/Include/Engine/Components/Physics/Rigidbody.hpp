#pragma once
#include <Engine/Log.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>

namespace Engine::Components
{
	struct Rigidbody : public Component
	{
		bool UseGravity = true;
		bool IsTrigger = false;

		float Mass = 1.0f;
		glm::vec3 Force = { 0, 0, 0 };
		glm::vec3 Velocity = { 0, 0, 0 };

		Physics::PhysicsSystem& GetSystem();

		void ApplyForce(glm::vec3 force);

	protected:
		void FixedUpdate(float timestep) override;
	};
}