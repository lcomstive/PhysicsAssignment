#pragma once
#include <Engine/Components/Physics/Particle.hpp>

namespace Engine::Components
{
	struct Spring : PhysicsComponent
	{
		Components::Particle* GetPoint1();
		Components::Particle* GetPoint2();

		void SetBodies(Components::Particle* a, Components::Particle* b);

		/// <param name="stiffness">How stiff the spring is. Range is [0, -inf]</param>
		/// <param name="dampening">Dampening force on the spring. Range is [0.0-1.0]</param>
		void SetConstants(float stiffness, float dampening);

		void SetRestingLength(float length);
		float GetRestingLength();

		void DrawGizmos() override;

	protected:
		Components::Particle* m_Point1;
		Components::Particle* m_Point2;

		float m_Stiffness = 0; // [0 - inf] Higher = stiffer
		float m_Dampening = 0.9f; // [0-1]
		float m_RestingLength = 1.0f;

		void ApplyForces(float timestep) override;
	};
}