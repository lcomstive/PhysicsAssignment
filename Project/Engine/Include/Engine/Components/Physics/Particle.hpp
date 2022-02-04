#pragma once
#include <Engine/Log.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>

namespace Engine::Components
{
	struct Particle : public Component
	{
		bool UseGravity = true;
		bool IsTrigger = false;

		Particle();

		Physics::PhysicsSystem& GetSystem();

		void ApplyForce(glm::vec3 force);

		float GetMass();
		void  SetMass(float value);

		float GetFriction();
		void  SetFriction(float value);

		float GetBounciness();
		void  SetBounciness(float value);

	protected:
		void Added() override;
		void FixedUpdate(float timestep) override;

	private:
		bool m_Initialised = false;

		glm::vec3 m_Force;
		float m_Mass, m_Bounciness, m_Friction;
		glm::vec3 m_PreviousPosition;

		void SolveConstraints(float timestep);
	};
}