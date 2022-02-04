#include <vector>
#include <Engine/Log.hpp>
#include <Engine/Utilities.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Physics/CollisionInfo.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/Physics/PlaneCollider.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Physics;
using namespace Engine::Components;

#pragma region Collision Tests
bool Engine::Physics::TestSphereBoxCollider(Sphere a, OBB b)
{
	vec3 closestPoint = b.GetClosestPoint(a.Position);
	return distance(closestPoint, a.Position) < a.Radius;
}

bool Engine::Physics::TestSphereBoxCollider(Sphere a, AABB b)
{
	vec3 closestPoint = b.GetClosestPoint(a.Position);
	return distance(closestPoint, a.Position) < a.Radius;
}

bool Engine::Physics::TestSphereSphereCollider(Sphere a, Sphere b)
{
	return distance(a.Position, b.Position) < (a.Radius + b.Radius);
}

bool Engine::Physics::TestSpherePlaneCollider(Sphere a, Plane b)
{
	vec3 closestPoint = b.GetClosestPoint(a.Position);
	return distance(closestPoint, a.Position) < a.Radius;
}

bool Engine::Physics::TestBoxBoxCollider(AABB a, OBB b) { return TestBoxBoxCollider(OBB { a.Position, a.Extents, mat4(1.0f) }, b); }
bool Engine::Physics::TestBoxBoxCollider(AABB a, AABB b)
{
	vec3 aMin = a.Min(), aMax = a.Max();
	vec3 bMin = b.Min(), bMax = b.Max();
	return aMin.x <= bMax.x && aMax.x >= bMin.x &&
		aMin.y <= bMax.y && aMax.y >= bMin.y &&
		aMin.z <= bMax.z && aMax.z >= bMin.z;
}

bool Engine::Physics::TestBoxBoxCollider(OBB a, OBB b)
{
	/// Separating Axis Theorem ///
	// Axes to test against
	vec3 test[15] =
	{
		a.Orientation[0],
		a.Orientation[1],
		a.Orientation[2],
		b.Orientation[0],
		b.Orientation[1],
		b.Orientation[2],
	};

	for (int i = 0; i < 3; i++)
	{
		test[6 + i * 3 + 0] = cross(test[i], test[0]);
		test[6 + i * 3 + 1] = cross(test[i], test[1]);
		test[6 + i * 3 + 2] = cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; i++)
		if (!a.OverlapOnAxis(b, test[i]))
			return false; // Separating axis found

	return true;
}

bool Engine::Physics::TestBoxPlaneCollider(OBB a, Plane b)
{
	vec3 rot[] = { a.Orientation[0], a.Orientation[1], a.Orientation[2] };
	float planeLength = a.Extents.x * fabsf(dot(b.Normal, rot[0])) +
						a.Extents.y * fabsf(dot(b.Normal, rot[1])) +
						a.Extents.z * fabsf(dot(b.Normal, rot[2]));
	float collisionDot = dot(b.Normal, a.Position);
	float t = fabsf(collisionDot - b.Distance) - planeLength;

	return t > 0;
}

bool Engine::Physics::TestPlanePlaneCollider(Plane a, Plane b)
{
	vec3 d = cross(a.Normal, b.Normal);
	return abs(dot(d, d)) < 0.0001f;
}
#pragma endregion

#pragma region Collision Manifolds
CollisionManifold Engine::Physics::FindCollisionFeatures(Sphere a, Sphere b)
{
	CollisionManifold result = {};
	float r = a.Radius + b.Radius;
	vec3 distance = b.Position - a.Position;

	if (!TestSphereSphereCollider(a, b))
		return result;

	// Normalize for direction
	distance = normalize(distance);

	result.IsColliding = true;
	result.Normal = distance;
	result.PenetrationDepth = fabsf(length(distance) - r) * 0.5f;

	// Distance to intersection point
	float dtp = a.Radius - result.PenetrationDepth;
	vec3 contact = a.Position + distance * dtp;
	result.Contacts.emplace_back(contact);

	return result;
}

CollisionManifold Engine::Physics::FindCollisionFeatures(OBB a, Sphere b)
{
	CollisionManifold result = {};
	vec3 closestPoint = a.GetClosestPoint(b.Position);
	float distance = length(closestPoint - b.Position);
	if (distance > b.Radius)
		return result; // No collision occurs

	vec3 normal;
	if (fabsf(distance) < 0.0001f)
	{
		// Closest point is at center of sphere,
		// try to find a new closest point

		float magnitude = length(closestPoint - a.Position);
		if (BasicallyZero(magnitude))
			return result;

		normal = normalize(closestPoint - a.Position);
	}
	else
		normal = normalize(b.Position - closestPoint);

	vec3 outsidePoint = b.Position - normal * b.Radius;
	distance = length(closestPoint - outsidePoint);

	result.Normal = normal;
	result.IsColliding = true;
	result.PenetrationDepth = distance * 0.5f;
	result.Contacts.emplace_back(outsidePoint + (outsidePoint - closestPoint) * 0.5f);

	return result;
}

CollisionManifold Engine::Physics::FindCollisionFeatures(OBB a, OBB b)
{
	CollisionManifold result = {};

	vec3 faceAxis[15] =
	{
		a.Orientation[0],
		a.Orientation[1],
		a.Orientation[2],
		b.Orientation[0],
		b.Orientation[1],
		b.Orientation[2],
	};
	for (int i = 0; i < 3; i++)
	{
		faceAxis[6 + i * 3 + 0] = cross(faceAxis[i], faceAxis[0]);
		faceAxis[6 + i * 3 + 1] = cross(faceAxis[i], faceAxis[1]);
		faceAxis[6 + i * 3 + 2] = cross(faceAxis[i], faceAxis[2]);
	}

	vec3* hitNormal = nullptr;
	bool shouldFlip;
	for (int i = 0; i < 15; i++)
	{
		if (length(faceAxis[i]) < 0.001f)
			continue;

		float depth = a.PenetrationDepth(b, faceAxis[i], &shouldFlip);

		if (depth <= 0) // No intersection occurred
			return result;
		if (depth < result.PenetrationDepth)
		{
			if (shouldFlip)
				faceAxis[i] = -faceAxis[i];
			result.PenetrationDepth = depth;
			hitNormal = &faceAxis[i];
		}
	}

	if (!hitNormal)
		return result; // No normal found, no intersection
	vec3 axis = normalize(*hitNormal);

	vector<vec3> c1 = a.ClipEdges(b.GetEdges());
	vector<vec3> c2 = b.ClipEdges(a.GetEdges());

	result.Contacts.reserve(c1.size() + c2.size());

	result.Contacts.insert(result.Contacts.end(), c1.begin(), c1.end());
	result.Contacts.insert(result.Contacts.end(), c2.begin(), c2.end());

	Interval i = a.GetInterval(axis);
	float distance = (i.Max - i.Min) * 0.5f - result.PenetrationDepth * 0.5f;
	vec3 pointOnPlane = a.Position + axis * distance;

	for (int i = (int)result.Contacts.size() - 1; i >= 0; i--)
	{
		vec3 contact = result.Contacts[i];
		result.Contacts[i] = contact + (axis * dot(axis, pointOnPlane - contact));

		// Check for duplicates
		for (int j = (int)result.Contacts.size() - 1; j > i; j--)
		{
			if (BasicallyZero(MagnitudeSqr(result.Contacts[j] - result.Contacts[i])))
			{
				result.Contacts.erase(result.Contacts.begin() + j);
				break;
			}
		}
	}

	result.Normal = axis;
	result.IsColliding = true;

	return result;
}

const unordered_map<type_index, unordered_map<type_index, function<CollisionManifold(Collider*, Collider*)>>> CollisionTests =
{
	{
		typeid(BoxCollider), // A
		unordered_map<type_index, function<CollisionManifold(Collider*, Collider*)>>
		{
			{ typeid(BoxCollider)	 /* B */, [](void* a, void* b) { return FindCollisionFeatures(((BoxCollider*)a)->BuildOBB(), ((BoxCollider*)b)->BuildOBB()); } },
			{ typeid(SphereCollider) /* B */, [](void* a, void* b) { return FindCollisionFeatures(((BoxCollider*)a)->BuildOBB(), ((SphereCollider*)b)->BuildSphere()); }}
		}
	},
	{
		typeid(SphereCollider), // A
		unordered_map<type_index, function<CollisionManifold(Collider*, Collider*)>>
		{
			{ typeid(BoxCollider)    /* B */, [](void* a, void* b) { return FindCollisionFeatures(((BoxCollider*)b)->BuildOBB(), ((SphereCollider*)a)->BuildSphere()); } },
			{ typeid(SphereCollider) /* B */, [](void* a, void* b) { return FindCollisionFeatures(((SphereCollider*)a)->BuildSphere(), ((SphereCollider*)b)->BuildSphere()); } }
		}
	}
};

CollisionManifold Engine::Physics::FindCollisionFeatures(Rigidbody* a, Rigidbody* b)
{
	Collider* aCollider = a->GetGameObject()->GetComponent<Collider>(true);
	Collider* bCollider = b->GetGameObject()->GetComponent<Collider>(true);

	Log::Assert(aCollider != nullptr && bCollider != nullptr, "Cannot find collision features without colliders!");

	type_index aColliderType = typeid(aCollider);
	type_index bColliderType = typeid(bCollider);

	Log::Debug("A Collider: " + string(aColliderType.name()));
	Log::Debug("B Collider: " + string(bColliderType.name()));

	const auto& itA = CollisionTests.find(aColliderType);
	if (itA != CollisionTests.end())
	{
		const auto& itB = itA->second.find(bColliderType);
		if (itB != itA->second.end())
			return itB->second(aCollider, bCollider);
	}

	Log::Warning("No valid collision manifold test found for '" + string(aColliderType.name()) + "' -> '" + string(bColliderType.name()) + "'");
	return CollisionManifold {};
}
#pragma endregion