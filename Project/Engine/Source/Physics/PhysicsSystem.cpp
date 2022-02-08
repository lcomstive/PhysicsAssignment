#include <chrono>
#include <functional>
#include <condition_variable>
#include <Engine/Application.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace std;
using namespace glm;
using namespace std::chrono;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

const vec3 InitialGravity = { 0, -9.81f, 0 };

PhysicsSystem::PhysicsSystem(Application* app, milliseconds fixedTimestep) :
	m_App(app),
	m_Thread(),
	m_Substeps(5),
	m_Solver(nullptr),
	m_CollidersMutex(),
	m_LastTimeStep(-1ms),
	m_ImpulseIteration(5),
	m_Broadphase(nullptr),
	m_Gravity(InitialGravity),
	m_FixedTimestep(fixedTimestep),
	m_ThreadState((int)(m_PhysicsState = PhysicsPlayState::Stopped))
{
	SetBroadphase<BasicBroadphase>();
}

PhysicsSystem::~PhysicsSystem()
{
	if (m_Solver)
		delete m_Solver;
	if (m_Broadphase)
		delete m_Broadphase;
}

void PhysicsSystem::SetTimestep(milliseconds timestep)
{
	lock_guard guard(m_VariableMutex);
	m_FixedTimestep = timestep;
}

void PhysicsSystem::SetGravity(vec3 gravity)
{
	lock_guard guard(m_VariableMutex);
	m_Gravity = gravity;
}

glm::vec3 PhysicsSystem::GetGravity() { return m_Gravity; }
std::chrono::milliseconds PhysicsSystem::LastTimeStep() { return m_LastTimeStep; }
milliseconds PhysicsSystem::Timestep() { return m_FixedTimestep; }

void PhysicsSystem::Start()
{
	if (!m_Solver)
		SetSolver<LinearProjectionSolver>();

	m_ThreadState.store((int)(m_PhysicsState = PhysicsPlayState::Playing));
	m_Thread = thread(&PhysicsSystem::PhysicsLoop, this);
}

void PhysicsSystem::Stop()
{
	m_ThreadState.store((int)(m_PhysicsState = PhysicsPlayState::Stopped));
	pauseConditional.notify_one();

	if (m_Thread.joinable())
		m_Thread.join();
}

void PhysicsSystem::Resume()
{
	lock_guard lock(m_PhysicsStateMutex);
	m_ThreadState.store((int)(m_PhysicsState = PhysicsPlayState::Playing));
	pauseConditional.notify_one();
}

void PhysicsSystem::Pause()
{
	lock_guard lock(m_PhysicsStateMutex);
	m_ThreadState.store((int)(m_PhysicsState = PhysicsPlayState::Paused));
	pauseConditional.notify_one();
}
void PhysicsSystem::TogglePause()
{
	if (m_PhysicsState == PhysicsPlayState::Playing)
		Pause();
	else
		Resume();
}

PhysicsPlayState PhysicsSystem::GetState() { return m_PhysicsState; }

void PhysicsSystem::AddCollider(Collider* collider)
{
	lock_guard guard(m_CollidersMutex);
	m_Colliders.emplace_back(collider);

	if (m_Broadphase)
		m_Broadphase->Insert(collider);
}

void PhysicsSystem::RemoveCollider(Collider* collider)
{
	const auto& it = find(m_Colliders.begin(), m_Colliders.end(), collider);
	if (it != m_Colliders.end())
	{
		lock_guard guard(m_CollidersMutex);
		m_Colliders.erase(it);
	}

	if (m_Broadphase)
		m_Broadphase->Remove(collider);
}

void PhysicsSystem::AddRigidbody(Rigidbody* rb)
{
	lock_guard guard(m_CollidersMutex);
	m_Rigidbodies.emplace_back(rb);
}

void PhysicsSystem::RemoveRigidbody(Rigidbody* rb)
{
	const auto& it = find(m_Rigidbodies.begin(), m_Rigidbodies.end(), rb);
	if (it != m_Rigidbodies.end())
	{
		lock_guard guard(m_CollidersMutex);
		m_Rigidbodies.erase(it);
	}
}

void PhysicsSystem::PhysicsLoop()
{
	PhysicsPlayState currentState;
	while ((currentState = (PhysicsPlayState)m_ThreadState.load()) != PhysicsPlayState::Stopped)
	{
		if (currentState == PhysicsPlayState::Paused)
		{
			unique_lock lock(m_PhysicsStateMutex);
			pauseConditional.wait(lock, [&] { return m_PhysicsState != PhysicsPlayState::Paused; });
		}

		Log::Assert(m_Broadphase, "A broadphase is required! Use PhysicsSystem::SetBroadphase to set one");

		time_point timeStart = high_resolution_clock::now();

		// Check for collisions
		vector<CollisionFrame>& collisions = m_Broadphase->GetPotentialCollisions();

		NarrowPhase(collisions); // Test potential collisions & generate manifolds

		// Apply additional external forces (like gravity)
		for (Collider* collider : m_Colliders)
		{
			Rigidbody* rb = collider->GetRigidbody();
			if (rb)
				rb->ApplyWorldForces();
		}

		// Apply linear impulse resolution
		for (int k = 0; k < m_ImpulseIteration; k++)
		{
			for (CollisionFrame collision : collisions)
			{
				int jSize = (int)collision.Result.Contacts.size();
				for (int j = 0; j < jSize; j++)
				{
					if (collision.BRigidbody && !collision.BRigidbody->IsStatic())
						collision.ARigidbody->ApplyImpulse(collision.BRigidbody, collision.Result, j);
					else // RB -> Static Collider
						collision.ARigidbody->ApplyImpulse(collision.B, collision.Result, j);
				}
			}
		}

		float fixedTimestep = m_FixedTimestep.count() / 1000.0f;
		for(Rigidbody* rb : m_Rigidbodies)
			rb->PreFixedUpdate(fixedTimestep);
		m_App->FixedStep(fixedTimestep); // milliseconds -> seconds

		RunSolver(collisions);

		// Solve constraints
		for (Collider* collider : m_Colliders)
		{
			Rigidbody* rb = collider->GetRigidbody();
			if (rb)
				rb->SolveConstraints(fixedTimestep);
		}

		m_LastTimeStep = duration_cast<milliseconds>(high_resolution_clock::now() - timeStart);
		milliseconds remainingTime = m_FixedTimestep - m_LastTimeStep;

		if (remainingTime.count() > 0)
			this_thread::sleep_for(remainingTime);
	}
}

void PhysicsSystem::RunSolver(vector<CollisionFrame>& collisions)
{
	if (!m_Solver)
		return;

	m_Solver->SetCollisions(collisions);

	m_Solver->PreSolve();
	m_Solver->Solve(m_FixedTimestep.count() / 1000.0f);
}

void PhysicsSystem::NarrowPhase(vector<CollisionFrame>& potentialCollisions)
{
	for (int i = (int)potentialCollisions.size() - 1; i >= 0; i--)
	{
		potentialCollisions[i].Result = FindCollisionFeatures(potentialCollisions[i].A, potentialCollisions[i].B);

		if (!potentialCollisions[i].Result.IsColliding)
		{
			potentialCollisions.erase(potentialCollisions.begin() + i);
			continue;
		}

		if (!potentialCollisions[i].ARigidbody || potentialCollisions[i].ARigidbody->IsStatic())
		{
			// A valid non-static rigidbody needs to be in A

			Collider* temp = potentialCollisions[i].A;
			potentialCollisions[i].A = potentialCollisions[i].B;
			potentialCollisions[i].ARigidbody = potentialCollisions[i].BRigidbody;
			potentialCollisions[i].B = temp;
			potentialCollisions[i].BRigidbody = nullptr;
			potentialCollisions[i].Result.Normal *= -1.0f;
		}
	}
}

Collider* PhysicsSystem::Raycast(Ray ray, RaycastHit* outResult) { return Raycast(ray, nullptr, outResult); }
vector<Collider*> PhysicsSystem::Query(AABB& bounds) { return m_Broadphase ? m_Broadphase->Query(bounds) : vector<Collider*>(); }
vector<Collider*> PhysicsSystem::Query(Sphere& bounds) { return m_Broadphase ? m_Broadphase->Query(bounds) : vector<Collider*>(); }
bool PhysicsSystem::LineTest(Line line, Engine::Components::Collider* ignoreCollider) { return m_Broadphase ? m_Broadphase->LineTest(line, ignoreCollider) : false; }
Collider* PhysicsSystem::Raycast(Ray ray, Collider* ignoreCollider, RaycastHit* outResult) { return m_Broadphase ? m_Broadphase->Raycast(ray, ignoreCollider, outResult) : nullptr; }

void PhysicsSystem::DrawGizmos()
{
	if (m_Broadphase)
		m_Broadphase->DrawGizmos();
}
