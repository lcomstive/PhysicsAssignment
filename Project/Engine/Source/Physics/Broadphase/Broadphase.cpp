#include <Engine/Utilities.hpp> // For MagnitudeSqr
#include <Engine/Components/Physics/Rigidbody.hpp>
#include <Engine/Physics/Broadphase/Broadphase.hpp>

using namespace std;
using namespace glm;
using namespace Engine::Physics;
using namespace Engine::Components;

void BasicBroadphase::Insert(Collider* collider) { m_Colliders.emplace_back(collider); }

void BasicBroadphase::Remove(Collider* collider)
{
	const auto& it = find(m_Colliders.begin(), m_Colliders.end(), collider);
	if (it != m_Colliders.end())
		m_Colliders.erase(it);
}

Collider* BasicBroadphase::GetCollider(vec3& point)
{
	for (Collider* collider : m_Colliders)
		if (collider->IsPointInside(point))
			return collider;
	return nullptr;
}

std::vector<CollisionFrame>& BasicBroadphase::GetPotentialCollisions()
{
	m_Collisions.clear();
	m_Collisions.reserve(m_Colliders.size() * 2); // Assume every object is colliding with something

	for (uint32_t i = 0, size = (uint32_t)m_Colliders.size(); i < size; i++)
	{
		Rigidbody* aRb = m_Colliders[i]->GetRigidbody();
		OBB& bounds = m_Colliders[i]->GetBounds();
		float magnitude = MagnitudeSqr(bounds.Extents);
		for (uint32_t j = i + 1; j < size; j++)
		{
			OBB& bounds2 = m_Colliders[j]->GetBounds();
			float dist = distance(bounds.Position, bounds2.Position);
			if (distance(bounds.Position, bounds2.Position) > magnitude)
				continue;

			Rigidbody* bRb = m_Colliders[j]->GetRigidbody();
			if (((aRb && !aRb->IsStatic()) || (bRb && !bRb->IsStatic())) &&
				TestBoxBoxCollider(bounds, bounds2))
				m_Collisions.emplace_back(CollisionFrame
					{
						m_Colliders[i],
						aRb,
						m_Colliders[j],
						bRb
					});
		}
	}

	return m_Collisions;
}

vector<Collider*> BasicBroadphase::Query(Sphere& bounds) const
{
	vector<Collider*> output;

	for (int i = 0; i < m_Colliders.size(); i++)
	{
		if (TestSphereBoxCollider(bounds, m_Colliders[i]->GetBounds()))
			output.emplace_back(m_Colliders[i]);
	}
	return output;
}

vector<Collider*> BasicBroadphase::Query(AABB& bounds) const
{
	vector<Collider*> output;

	for (int i = 0; i < m_Colliders.size(); i++)
	{
		if (TestBoxBoxCollider(bounds, m_Colliders[i]->GetBounds()))
			output.emplace_back(m_Colliders[i]);
	}
	return output;
}

bool BasicBroadphase::LineTest(Line& line, Collider* ignoreCollider)
{
	Collider* closest = nullptr;
	RaycastHit hit = {}, closestHit = {};

	for (int i = 0; i < m_Colliders.size(); i++)
	{
		if (m_Colliders[i] == ignoreCollider ||
			!m_Colliders[i]->LineTest(line) ||
			(closest && hit.Distance >= closestHit.Distance))
			continue;
		closest = m_Colliders[i];
		closestHit = hit;
	}

	return closest;
}

Collider* BasicBroadphase::Raycast(Ray ray, Collider* ignoreCollider, RaycastHit* outResult) const
{
	Collider* closest = nullptr;
	RaycastHit hit = {}, closestHit = {};

	for (int i = 0; i < m_Colliders.size(); i++)
	{
		if (m_Colliders[i] == ignoreCollider ||
			!m_Colliders[i]->Raycast(ray, &hit) ||
			(closest && hit.Distance >= closestHit.Distance))
			continue;
		closest = m_Colliders[i];
		closestHit = hit;
	}

	if (outResult)
		*outResult = closestHit;
	return closest;
}
