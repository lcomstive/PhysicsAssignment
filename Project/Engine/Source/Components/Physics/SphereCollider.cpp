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
	Gizmos::DrawWireSphere(GetTransform()->Position, Radius * GetTransform()->Scale.x);

	OBB bounds = GetBounds();
	Gizmos::DrawWireCube(bounds.Position, bounds.Extents, eulerAngles(quat(bounds.Orientation)));
#endif
}

Sphere SphereCollider::BuildSphere() const
{
	Transform* transform = GetTransform();
	return Sphere
	{
		Offset + transform->Position,
		Radius * transform->Scale.x
	};
}

OBB SphereCollider::GetBounds() const
{
	Transform* transform = GetTransform();
	return OBB
	{
		transform->Position + Offset,
		transform->Scale * Radius,
		mat4(1.0f) // Rotation
	};
};
bool SphereCollider::LineTest(Line& line) { return BuildSphere().LineTest(line); }
bool SphereCollider::IsPointInside(glm::vec3 point) const { return BuildSphere().IsPointInside(point); }
bool SphereCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildSphere().Raycast(ray, outResult); }

bool SphereCollider::CheckCollision(const Collider* other) const { return other->CheckCollision(this); }

bool SphereCollider::CheckCollision(const BoxCollider* other) const
{
	return TestSphereBoxCollider(
		BuildSphere(),
		other->BuildOBB()
	);
}

bool SphereCollider::CheckCollision(const SphereCollider* other) const
{
	return TestSphereSphereCollider(
		BuildSphere(),
		other->BuildSphere()
	);
}

bool SphereCollider::CheckCollision(const PlaneCollider* other) const
{
	return TestSpherePlaneCollider(
		BuildSphere(),
		other->BuildPlane()
	);
}