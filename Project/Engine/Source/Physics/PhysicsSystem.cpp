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
	m_Octree(nullptr),
	m_ImpulseIteration(10),
	m_PreviousCollisions(),
	m_Gravity(InitialGravity),
	m_PenetrationSlack(0.5f),
	m_FixedTimestep(fixedTimestep),
	m_LinearProjectionPercent(0.45f),
	m_ThreadState((int)(m_PhysicsState = PhysicsPlayState::Stopped))
{
	// TODO: Fine tune this, or have function for variability
	m_Collisions.reserve(100);
}

PhysicsSystem::~PhysicsSystem()
{
	if (m_Octree)
		delete m_Octree;
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

milliseconds  PhysicsSystem::Timestep() { return m_FixedTimestep; }
glm::vec3 PhysicsSystem::GetGravity() { return m_Gravity; }

void PhysicsSystem::Start()
{
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

bool PhysicsSystem::Accelerate(vec3& position, float size, int maxRecursions)
{
	if (m_Octree)
		return false;

	vec3 min = position - vec3 { size, size, size };
	vec3 max = position + vec3 { size, size, size };

	m_Octree = new OctreeNode();
	m_Octree->Bounds = AABB::FromMinMax(min, max);
	m_Octree->Children = nullptr;
	for (uint32_t i = 0; i < m_Colliders.size(); i++)
		m_Octree->Colliders.emplace_back(m_Colliders[i]);
	SplitTree(m_Octree, maxRecursions);
	return true;
}

void PhysicsSystem::AddCollider(Collider* collider)
{
	lock_guard guard(m_CollidersMutex);
	m_Colliders.emplace_back(collider);
	
	if (m_Octree)
		Insert(m_Octree, collider);
}

void PhysicsSystem::RemoveCollider(Collider* collider)
{
	const auto& it = find(m_Colliders.begin(), m_Colliders.end(), collider);
	if (it != m_Colliders.end())
	{
		lock_guard guard(m_CollidersMutex);
		m_Colliders.erase(it);
	}

	if (m_Octree)
		Remove(m_Octree, collider);
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

		auto timeStart = high_resolution_clock::now();

		m_PreviousCollisions = m_Collisions;
		m_Collisions.clear();

		// Check for collisions
		vector<pair<int, int>> collisionIndices = {};
		for (int i = 0, size = (int)m_Colliders.size(); i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (i == j)
					continue;
				
				// Check for duplicates
				bool duplicate = false;
				for(pair<int, int>& index : collisionIndices)
				{
					if((index.first == i && index.second == j) ||
					   (index.first == j && index.second == i))
					{
						duplicate = true;
						break;
					}
				}
				if(duplicate)
					continue;
						
				Rigidbody* aRb = m_Colliders[i]->GetRigidbody();
				if(!aRb)
					continue; // Collider needs rigidbody to calculate results

				CollisionManifold result = FindCollisionFeatures(m_Colliders[i], m_Colliders[j]);
				if (!result.IsColliding)
					continue;

				m_Collisions.emplace_back(CollisionFrame
				{
					m_Colliders[i],
					aRb,
					m_Colliders[j],
					m_Colliders[j]->GetRigidbody(),
					result
				});
				
				collisionIndices.emplace_back(make_pair(i, j));
			}
		}

		// Apply additional external forces (like gravity)
		for (Collider* collider : m_Colliders)
		{
			Rigidbody* rb = collider->GetRigidbody();
			if(rb)
				rb->ApplyWorldForces();
		}

		// Apply linear impulse resolution
		for (int k = 0; k < m_ImpulseIteration; k++)
		{
			for(CollisionFrame collision : m_Collisions)
			{
				int jSize = (int)collision.Result.Contacts.size();
				for (int j = 0; j < jSize; j++)
				{
					if(collision.BRigidbody && !collision.BRigidbody->IsStatic())
						collision.ARigidbody->ApplyImpulse(collision.BRigidbody, collision.Result, j);
					else // RB -> Static Collider
						collision.ARigidbody->ApplyImpulse(collision.B, collision.Result, j);
				}
			}
		}

		m_App->FixedStep(m_FixedTimestep.count() / 1000.0f); // milliseconds -> seconds

		// Positional correction
		for(CollisionFrame collision : m_Collisions)
		{
			if(collision.ARigidbody->IsStatic() || !collision.BRigidbody || collision.BRigidbody->IsStatic())
				continue;
			
			float totalMass = collision.ARigidbody->InverseMass() + collision.BRigidbody->InverseMass();
			if(totalMass == 0.0f)
				continue;
			
			float depth = fmaxf(collision.Result.PenetrationDepth - m_PenetrationSlack, 0.0f);
			float scalar = (totalMass == 0.0f) ? 0.0f : depth / totalMass;
			vec3 correction = collision.Result.Normal * scalar * m_LinearProjectionPercent;
			
			collision.A->GetTransform()->Position -= correction * collision.ARigidbody->InverseMass();
			
			collision.B->GetTransform()->Position += correction * collision.BRigidbody->InverseMass();
		}

		// Solve constraints
		for (Collider* collider : m_Colliders)
		{
			Rigidbody* rb = collider->GetRigidbody();
			if (rb)
				rb->SolveConstraints(m_FixedTimestep.count() / 1000.0f);
		}

		milliseconds remainingTime = duration_cast<milliseconds>(m_FixedTimestep - (high_resolution_clock::now() - timeStart));
		if (remainingTime.count() > 0)
			this_thread::sleep_for(remainingTime);
	}
}

Collider* PhysicsSystem::Raycast(Ray& ray, RaycastHit* outResult) { return Raycast(ray, nullptr, outResult); }
Collider* PhysicsSystem::Raycast(Ray& ray, Collider* ignoreCollider, RaycastHit* outResult)
{
	if (m_Octree)
		return Raycast(m_Octree, ray, ignoreCollider, outResult);

	Collider* closest = nullptr;
	RaycastHit hit = {}, closestHit = {};

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		if (colliders[i] == ignoreCollider ||
			!colliders[i]->Raycast(ray, &hit) ||
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
	if (m_Octree)
		return Query(m_Octree, bounds);

	vector<Collider*> output;

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		if (TestSphereBoxCollider(bounds, colliders[i]->GetBounds()))
			output.emplace_back(colliders[i]);
	}
	return output;
}

vector<Collider*> PhysicsSystem::Query(AABB& bounds)
{
	if (m_Octree)
		return Query(m_Octree, bounds);

	vector<Collider*> output;

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		if (TestBoxBoxCollider(bounds, colliders[i]->GetBounds()))
			output.emplace_back(colliders[i]);
	}
	return output;
}

Collider* PhysicsSystem::FindClosest(std::vector<Collider*>& set, Ray& ray, RaycastHit* outResult)
{
	if (set.size() == 0)
		return nullptr;

	Collider* closest = nullptr;
	float closestDistance = 99999;
	for (uint32_t i = 0; i < (uint32_t)set.size(); i++)
	{
		RaycastHit hit;
		if (!set[i]->Raycast(ray, &hit) ||
			hit.Distance >= closestDistance)
			continue;
		closest = set[i];
		closestDistance = hit.Distance;

		if (outResult)
			*outResult = hit;
	}
	return closest;
}

Collider* PhysicsSystem::Raycast(OctreeNode* node, Ray& ray, RaycastHit* outResult) { return Raycast(node, ray, nullptr, outResult); }
Collider* PhysicsSystem::Raycast(OctreeNode* node, Ray& ray, Collider* ignoreCollider, RaycastHit* outResult)
{
	if (outResult)
		*outResult = {}; // Set to initial values

	RaycastHit hit;
	if (!node->Bounds.Raycast(ray, &hit))
		return nullptr;
	if (!node->Children) // Leaf node, check colliders
		return FindClosest(node->Colliders, ray, outResult);

	vector<Collider*> results;
	for (int i = 0; i < 8; i++)
	{
		Collider* result = Raycast(&node->Children[i], ray, outResult);
		if (result && result != ignoreCollider)
			results.emplace_back(result);
	}

	// Find closest result
	return FindClosest(results, ray, outResult);
}

vector<Collider*> PhysicsSystem::Query(OctreeNode* node, AABB& box)
{
	vector<Collider*> results;
	if (!TestBoxBoxCollider(box, node->Bounds))
		return results;
	if (node->Children)
	{
		for (int i = 0; i < 8; i++)
		{
			vector<Collider*> child = Query(&node[i], box);
			if (child.size() > 0)
				results.insert(results.end(), child.begin(), child.end());
		}
	}
	else
	{
		for (uint32_t i = 0; i < (uint32_t)node->Colliders.size(); i++)
		{
			OBB bounds = node->Colliders[i]->GetBounds();
			if (TestBoxBoxCollider(box, bounds))
				results.emplace_back(node->Colliders[i]);
		}
	}
	return results;
}

vector<Collider*> PhysicsSystem::Query(OctreeNode* node, Sphere& sphere)
{
	vector<Collider*> results;
	if (!TestSphereBoxCollider(sphere, node->Bounds))
		return results;
	if (node->Children)
	{
		for (int i = 0; i < 8; i++)
		{
			vector<Collider*> child = Query(&node[i], sphere);
			if (child.size() > 0)
				results.insert(results.end(), child.begin(), child.end());
		}
	}
	else
	{
		for (uint32_t i = 0; i < (uint32_t)node->Colliders.size(); i++)
		{
			OBB bounds = node->Colliders[i]->GetBounds();
			if (TestSphereBoxCollider(sphere, bounds))
				results.emplace_back(node->Colliders[i]);
		}
	}
	return results;
}

bool PhysicsSystem::LineTest(OctreeNode* node, Line& line, Engine::Components::Collider* ignoreCollider)
{
	if (!node->Bounds.LineTest(line))
		return false;
	if (node->Children) // Leaf node, check colliders
	{
		for (int i = 0; i < 8; i++)
			if (LineTest(&node->Children[i], line, ignoreCollider))
				return true;
		return false;
	}

	for (uint32_t i = 0; i < (uint32_t)node->Colliders.size(); i++)
	{
		if (node->Colliders[i] != ignoreCollider &&
			node->Colliders[i]->LineTest(line))
			return true;
	}
	return false;
}

bool PhysicsSystem::LineTest(Line line, Engine::Components::Collider* ignoreCollider)
{
	if (m_Octree)
		return LineTest(m_Octree, line, ignoreCollider);

	Collider* closest = nullptr;
	RaycastHit hit = {}, closestHit = {};

	m_CollidersMutex.lock();
	vector<Collider*> colliders = m_Colliders;
	m_CollidersMutex.unlock();

	for (int i = 0; i < colliders.size(); i++)
	{
		if (colliders[i] == ignoreCollider ||
			!colliders[i]->LineTest(line) ||
			(closest && hit.Distance >= closestHit.Distance))
			continue;
		closest = colliders[i];
		closestHit = hit;
	}

	return closest;
}

void PhysicsSystem::DrawGizmos()
{
	for(CollisionFrame collision : m_PreviousCollisions)
	{
		// Draw collision normal
		Gizmos::Colour = { 1, 0, 1, 1 };
		vec3 position = collision.A->GetTransform()->Position;
		Gizmos::DrawCube(position, vec3 { 0.01f, 0.01f, 0.01f } + collision.Result.Normal);
		
		// Draw contact points
		Gizmos::Colour = { 0, 1, 1, 1 };
		for(vec3 contactPoint : collision.Result.Contacts)
			Gizmos::DrawSphere(contactPoint, 0.05f);
	}
}
