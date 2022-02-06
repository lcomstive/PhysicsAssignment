#pragma once
#include <Engine/Log.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>
#include <Engine/Physics/CollisionInfo.hpp>

namespace Engine::Components
{
	enum class ForceMode { Impulse, Acceleration };

	struct Rigidbody : public Component
	{
		bool UseGravity = true;
		bool IsTrigger = false;

		Rigidbody();

		Physics::PhysicsSystem& GetSystem();

		void ApplyForce(glm::vec3 force, ForceMode mode = ForceMode::Acceleration);

		float GetMass();
		void  SetMass(float value);

		float GetRestitution();
		void  SetRestitution(float value);

		float GetFriction();
		void  SetFriction(float value);
		
		bool IsStatic();
		void SetStatic(bool isStatic);

		float InverseMass();
		float PotentialEnergy();

	protected:
		void FixedUpdate(float timestep) override;

	private:
		bool m_IsStatic = false;
		bool m_Initialised = false;

		/// <summary>
		/// Sum of all acting forces this physics step
		/// </summary>
		glm::vec3 m_Force;
		
		/// <summary>
		/// Current motion of object
		/// </summary>
		glm::vec3 m_Velocity;

		/// <summary>
		/// Position of object in previous physics step,
		/// useful to prevent tunnelling (going through objects when in motion)
		/// </summary>
		glm::vec3 m_PreviousPosition;

		/// <summary>
		/// Mass of object
		/// </summary>
		float m_Mass;

		/// <summary>
		/// Coefficient of Restitution.
		/// Models bouncing collisions between two objects
		/// </summary>
		float m_CoR;

		/// <summary>
		/// Friction against colliding objects
		/// </summary>
		float m_Friction;

		void ApplyWorldForces();
		void SolveConstraints(float timestep);
		void ApplyImpulse(Collider* other, Physics::CollisionManifold manifold, int contactIndex);
		void ApplyImpulse(Rigidbody* other, Physics::CollisionManifold manifold, int contactIndex);

		friend class Engine::Physics::PhysicsSystem;
	};
}
