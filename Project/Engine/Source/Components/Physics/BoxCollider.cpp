#include <glm/gtx/euler_angles.hpp>
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>
#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/Physics/PlaneCollider.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>

using namespace glm;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

void BoxCollider::DrawGizmos()
{
#ifndef NDEBUG
	Transform* transform = GetTransform();
	Gizmos::Colour = { 0, 1, 0, 1 };
	Gizmos::DrawWireCube(
		transform->Position + Offset,
		transform->Scale * Extents * 2.0f,
		transform->Rotation);
#endif
}

OBB BoxCollider::BuildOBB() const
{
	Transform* transform = GetTransform();
	return OBB
	{
		transform->Position + Offset,
		Extents * transform->Scale,
		orientate3(transform->Rotation)
	};
}

bool BoxCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildOBB().Raycast(ray, outResult); }

OBB BoxCollider::GetBounds() const { return BuildOBB(); }
bool BoxCollider::IsPointInside(glm::vec3 point) const { return BuildOBB().IsPointInside(point); }

Collision BoxCollider::CheckCollision(const Collider* other) const { return other->CheckCollision(this); }

Collision BoxCollider::CheckCollision(const BoxCollider* other) const
{
	return TestBoxBoxCollider(
		BuildOBB(),
		GetRigidbody(),
		other->BuildOBB(),
		other->GetRigidbody()
	);
}

Collision BoxCollider::CheckCollision(const SphereCollider* other) const
{
	return TestSphereBoxCollider(
		other->BuildSphere(),
		other->GetRigidbody(),
		BuildOBB(),
		GetRigidbody()
	);
}

Collision BoxCollider::CheckCollision(const PlaneCollider* other) const
{
	return TestBoxPlaneCollider(
		BuildOBB(),
		GetRigidbody(),
		other->BuildPlane(),
		other->GetRigidbody()
	);
}
