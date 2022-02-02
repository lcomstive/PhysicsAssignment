#include <Engine/GameObject.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Physics/CollisionInfo.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace glm;
using namespace Engine::Physics;
using namespace Engine::Components;

Collision Engine::Physics::TestSphereBoxCollider(Sphere a, Rigidbody* aRb, OBB b, Rigidbody* bRb)
{
	Collision collision = { aRb, bRb };
	vec3 closestPoint = b.GetClosestPoint(a.Position);
	collision.Info.HasCollided = distance(closestPoint, a.Position) < a.Radius;
	return collision;
}

Collision Engine::Physics::TestSphereSphereCollider(Sphere a, Rigidbody* aRb, Sphere b, Rigidbody* bRb)
{
	Collision collision = { aRb, bRb };
	collision.Info.HasCollided = distance(a.Position, b.Position) < (a.Radius + b.Radius);
	return collision;
}

Collision Engine::Physics::TestSpherePlaneCollider(Sphere a, Rigidbody* aRb, Plane b, Rigidbody* bRb)
{
	Collision collision =
	{
		aRb,
		bRb
	};
	vec3 closestPoint = b.GetClosestPoint(a.Position);
	collision.Info.HasCollided = distance(closestPoint, a.Position) < a.Radius;
	return collision;
}

Collision Engine::Physics::TestBoxBoxCollider(AABB a, Rigidbody* aRb, OBB b, Rigidbody* bRb)
{
	return TestBoxBoxCollider(OBB { a.Position, a.Extents, mat4(1.0f) }, aRb, b, bRb);
}

Collision Engine::Physics::TestBoxBoxCollider(OBB a, Rigidbody* aRb, OBB b, Rigidbody* bRb)
{
	Collision collision = { aRb, bRb };

	// Separating Axis Theorem

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
			return collision; // Separating axis found

	collision.Info.HasCollided = true;
	return collision;
}

Collision Engine::Physics::TestBoxPlaneCollider(OBB a, Rigidbody* aRb, Plane b, Rigidbody* bRb)
{
	Collision collision = { aRb, bRb };
	vec3 rot[] = { a.Orientation[0], a.Orientation[1], a.Orientation[2] };
	float planeLength = a.Extents.x * fabsf(dot(b.Normal, rot[0])) +
						a.Extents.y * fabsf(dot(b.Normal, rot[1])) +
						a.Extents.z * fabsf(dot(b.Normal, rot[2]));
	float collisionDot = dot(b.Normal, a.Position);
	collision.Info.Depth = fabsf(collisionDot - b.Distance) - planeLength;
	collision.Info.HasCollided = collision.Info.Depth > 0;

	return collision;
}

Collision Engine::Physics::TestPlanePlaneCollider(Plane a, Rigidbody* aRb, Plane b, Rigidbody* bRb)
{
	Collision collision = { aRb, bRb };
	vec3 d = cross(a.Normal, b.Normal);
	collision.Info.HasCollided = abs(dot(d, d)) < 0.0001f;
	return collision;
}