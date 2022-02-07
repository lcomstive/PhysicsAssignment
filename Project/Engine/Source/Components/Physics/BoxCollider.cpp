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
		eulerAngleXYZ(transform->Rotation.x, transform->Rotation.y, transform->Rotation.z)
	};
}

bool BoxCollider::LineTest(Physics::Line& line) { return BuildOBB().LineTest(line); }
bool BoxCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildOBB().Raycast(ray, outResult); }

OBB BoxCollider::GetBounds() const { return BuildOBB(); }
bool BoxCollider::IsPointInside(glm::vec3 point) const { return BuildOBB().IsPointInside(point); }

mat4 BoxCollider::InverseTensor()
{
	Rigidbody* rb = GetRigidbody();
	float mass = 0.0f;
	if (!rb || (mass = rb->GetMass()) == 0.0f)
		return mat4(0.0f);

	vec3 size = Extents * 2.0f;
	float fraction = (1.0f / 12.0f);

	float x2 = size.x * size.x;
	float y2 = size.y * size.y;
	float z2 = size.z * size.z;

	return inverse(mat4(
		(y2 + z2) * mass * fraction, 0, 0, 0,
		0, (x2 + z2) * mass * fraction, 0, 0,
		0, 0, (x2 + y2) * mass * fraction, 0,
		0, 0, 0, 1.0f
	));
}

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
