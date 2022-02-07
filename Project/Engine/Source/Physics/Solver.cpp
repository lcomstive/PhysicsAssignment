#include <Engine/Physics/Solver.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Physics/PhysicsSystem.hpp> // CollisionFrame definition
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace std;
using namespace glm;
using namespace Engine::Physics;

#pragma region Solver
Solver::Solver() : m_Collisions() { }

void Solver::SetCollisions(vector<CollisionFrame> collisions) { m_Collisions = collisions; }
#pragma endregion

#pragma region Linear Projection
LinearProjectionSolver::LinearProjectionSolver(unsigned int maxIterations) : m_MaxIterations(maxIterations) { }

void LinearProjectionSolver::Solve(float timestep)
{
	if(m_Collisions.empty())
		return;
	
	for (unsigned int i = 0; i < m_MaxIterations; i++)
	{
		float strength = 1.0f / m_MaxIterations;
		for (CollisionFrame collision : m_Collisions)
		{
			if (collision.ARigidbody->IsStatic())
				continue;
			bool dynamicB = collision.BRigidbody && !collision.BRigidbody->IsStatic();

			float totalMass = collision.ARigidbody->InverseMass();
			if (dynamicB)
				totalMass += collision.BRigidbody->InverseMass();
			if (totalMass == 0.0f)
				continue;

			float depth = fmaxf(collision.Result.PenetrationDepth - m_PenetrationSlack, 0.0f);
			float scalar = (totalMass == 0.0f) ? 0.0f : depth / totalMass;
			vec3 correction = collision.Result.Normal * scalar * m_LinearProjectionPercent;
			correction *= strength;

			collision.A->GetTransform()->Position -= correction * (dynamicB ? collision.ARigidbody->InverseMass() : 1.0f);
			if (dynamicB)
				collision.B->GetTransform()->Position += correction * collision.BRigidbody->InverseMass();
		}
	}
}
#pragma endregion

#pragma region Gauss-Seidel
GSSolver::GSSolver(unsigned int maxIterations) : m_MaxIterations(maxIterations) { }

void GSSolver::Solve(float timestep)
{
	for (unsigned int i = 0; i < m_MaxIterations; i++)
	{

	}
}
#pragma endregion