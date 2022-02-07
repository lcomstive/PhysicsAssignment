#pragma once
#include <vector>
#include <Engine/Physics/CollisionInfo.hpp>

namespace Engine::Physics
{
	struct CollisionFrame; // Forward declaration

	class Solver
	{
	protected:
		std::vector<CollisionFrame> m_Collisions;

	public:
		Solver();

		void SetCollisions(std::vector<CollisionFrame> collisions);

		virtual void PreSolve() { }
		virtual void Solve(float timestep) = 0;
	};

	class LinearProjectionSolver : public Solver
	{
		unsigned int m_MaxIterations;

	public:
		/// <summary>
		/// How much positional correction to apply,
		/// smaller values allow objects to penetrate more.
		/// Typically 0.2-0.8
		/// </summary>
		float m_LinearProjectionPercent = 0.5f;

		/// <summary>
		/// How much to allow objects to penetrate.
		/// The larger the number, the less jitter in the system.
		/// Range 0.01-0.1.
		/// </summary>
		float m_PenetrationSlack = 0.05f;

		LinearProjectionSolver(unsigned int maxIterations = 10);
		 
		void Solve(float timestep) override;
	};

	// Gauss-Seidel
	class GSSolver : public Solver
	{
		unsigned int m_MaxIterations;

	public:
		GSSolver(unsigned int maxIterations = 10);
		 
		void Solve(float timestep) override;
	};
}