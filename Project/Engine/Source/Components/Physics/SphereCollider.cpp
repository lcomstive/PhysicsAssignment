#include <glm/gtc/quaternion.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/Physics/PlaneCollider.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>

using namespace glm;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

void SphereCollider::DrawGizmos()
{
#ifndef NDEBUG
	Gizmos::Colour = { 0, 1, 0, 1 };
	Gizmos::DrawWireSphere(GetTransform()->Position, Radius);

	OBB bounds = GetBounds();
	Gizmos::DrawWireCube(bounds.Position, bounds.Extents, eulerAngles(quat(bounds.Orientation)));
#endif
}

Sphere SphereCollider::BuildSphere() const
{
	return Sphere
	{
		GetTransform()->Position + Offset,
		Radius
	};
}

OBB SphereCollider::GetBounds() const
{
	return OBB
	{
		GetTransform()->Position + Offset,
		vec3 { Radius, Radius, Radius },
		mat4(1.0f) // Rotation
	};
};
bool SphereCollider::IsPointInside(glm::vec3 point) const { return BuildSphere().IsPointInside(point); }
bool SphereCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildSphere().Raycast(ray, outResult); }

Collision SphereCollider::CheckCollision(const Collider* other) const { return other->CheckCollision(this); }

Collision SphereCollider::CheckCollision(const BoxCollider* other) const
{
	return TestSphereBoxCollider(
		BuildSphere(),
		GetRigidbody(),
		other->BuildOBB(),
		other->GetRigidbody()
	);
}

Collision SphereCollider::CheckCollision(const SphereCollider* other) const
{
	return TestSphereSphereCollider(
		BuildSphere(),
		GetRigidbody(),
		other->BuildSphere(),
		other->GetRigidbody()
	);
}

Collision SphereCollider::CheckCollision(const PlaneCollider* other) const
{
	return TestSpherePlaneCollider(
		BuildSphere(),
		GetRigidbody(),
		other->BuildPlane(),
		other->GetRigidbody()
	);
}