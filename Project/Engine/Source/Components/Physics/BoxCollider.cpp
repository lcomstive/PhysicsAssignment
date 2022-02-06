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
		transform->Scale * Extents,
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

bool BoxCollider::LineTest(Physics::Line& line) { return BuildOBB().LineTest(line); }
bool BoxCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildOBB().Raycast(ray, outResult); }

OBB BoxCollider::GetBounds() const { return BuildOBB(); }
bool BoxCollider::IsPointInside(glm::vec3 point) const { return BuildOBB().IsPointInside(point); }

bool BoxCollider::CheckCollision(const Collider* other) const { return other->CheckCollision(this); }

bool BoxCollider::CheckCollision(const BoxCollider* other) const
{
	return TestBoxBoxCollider(
		BuildOBB(),
		other->BuildOBB()
	);
}

bool BoxCollider::CheckCollision(const SphereCollider* other) const
{
	return TestSphereBoxCollider(
		other->BuildSphere(),
		BuildOBB()
	);
}

bool BoxCollider::CheckCollision(const PlaneCollider* other) const
{
	return TestBoxPlaneCollider(
		BuildOBB(),
		other->BuildPlane()
	);
}