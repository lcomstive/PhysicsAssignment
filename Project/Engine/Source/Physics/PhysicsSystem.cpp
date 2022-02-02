#include <chrono>
#include <functional>
#include <Engine/Application.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace std;
using namespace glm;
using namespace std::chrono;
using namespace Engine::Physics;
using namespace Engine::Components;

PhysicsSystem::PhysicsSystem(Application* app, milliseconds fixedTimestep) : m_App(app), m_FixedTimestep(fixedTimestep), m_Thread(), m_KeepThreadAlive(false) { }

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

milliseconds  PhysicsSystem::Timestep() { return m_FixedTimestep; }
glm::vec3 PhysicsSystem::GetGravity() { return m_Gravity; }

void PhysicsSystem::Start()
{
	m_KeepThreadAlive.store(true);
	m_Thread = thread(&PhysicsSystem::PhysicsLoop, this);
}

void PhysicsSystem::Stop()
{
	m_KeepThreadAlive.store(false);
	if (m_Thread.joinable())
		m_Thread.join();
}

void PhysicsSystem::AddCollider(Collider* collider)
{
	lock_guard guard(m_CollidersMutex);
	m_Colliders.emplace_back(collider);
}

void PhysicsSystem::RemoveCollider(Collider* collider)
{
	const auto& it = find(m_Colliders.begin(), m_Colliders.end(), collider);
	if (it != m_Colliders.end())
	{
		lock_guard guard(m_CollidersMutex);
		m_Colliders.erase(it);
	}
}

void PhysicsSystem::PhysicsLoop()
{
	while (m_KeepThreadAlive.load())
	{
		auto timeStart = high_resolution_clock::now();

		m_App->FixedStep(m_FixedTimestep.count() / 1000.0f); // milliseconds -> seconds

		milliseconds remainingTime = duration_cast<milliseconds>(m_FixedTimestep - (high_resolution_clock::now() - timeStart));
		if (remainingTime.count() > 0)
			this_thread::sleep_for(remainingTime);
	}
}

void PhysicsSystem::PhysicsState::Recalculate()
{
	Velocity = Momentum * InverseMass;
}

Collider* PhysicsSystem::Raycast(Ray& ray, RaycastHit* outResult)
{
	Collider* closest = nullptr;
	RaycastHit hit = {}, closestHit = {};

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		if (!colliders[i]->Raycast(ray, &hit) ||
			(closest && hit.Distance >= closestHit.Distance))
			continue;
		closest = colliders[i];
		closestHit = hit;
	}

	if (outResult)
		*outResult = closestHit;
	return closest;
}

vector<Collider*> PhysicsSystem::Query(Sphere& bounds)
{
	vector<Collider*> output;

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		Collision collision = TestSphereBoxCollider(bounds, nullptr, colliders[i]->GetBounds(), nullptr);
		if (collision.Info.HasCollided)
			output.emplace_back(colliders[i]);
	}
	return output;
}

vector<Collider*> PhysicsSystem::Query(AABB& bounds)
{
	vector<Collider*> output;

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		Collision collision = TestBoxBoxCollider(bounds, nullptr, colliders[i]->GetBounds(), nullptr);
		if (collision.Info.HasCollided)
			output.emplace_back(colliders[i]);
	}
	return output;
}