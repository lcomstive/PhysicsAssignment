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
		bool CanSleep = true;
		bool UseGravity = true;
		bool IsTrigger = false;

		Rigidbody();

		Physics::PhysicsSystem& GetSystem();

		void ApplyForce(glm::vec3 force, ForceMode mode = ForceMode::Acceleration);
		void AddRotationalImpulse(glm::vec3 point, glm::vec3 impulse);

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

	private:
		bool m_IsStatic = false;

		/// <summary>
		/// Sum of all acting forces this physics step
		/// </summary>
		glm::vec3 m_Force;

		/// <summary>
		/// Sum of all torque acting this physics step
		/// </summary>
		glm::vec3 m_Torque;
		
		/// <summary>
		/// Current motion of object
		/// </summary>
		glm::vec3 m_Velocity;

		/// <summary>
		/// Current angular motion of object
		/// </summary>
		glm::vec3 m_AngularVelocity;

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

		/// <summary>
		/// When sleeping, no impulse resolution is applied until an external force is applied
		/// </summary>
		bool m_Sleeping = false;

		void Added() override;
		void Removed() override;

		glm::mat4& InverseTensor();

		void CheckSleeping();
		void ApplyWorldForces();
		void PreFixedUpdate(float timestep);
		void SolveConstraints(float timestep);
		void ApplyImpulse(Collider* other, Physics::CollisionManifold manifold, int contactIndex);
		void ApplyImpulse(Rigidbody* other, Physics::CollisionManifold manifold, int contactIndex);

		friend class Engine::Physics::PhysicsSystem;
	};
}
