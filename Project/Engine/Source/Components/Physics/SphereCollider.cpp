#include <glm/gtc/quaternion.hpp>
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

void SphereCollider::DrawGizmos()
{
#ifndef NDEBUG
	Gizmos::Colour = { 0, 1, 0, 1 };
	Gizmos::DrawWireSphere(GetTransform()->Position, Radius);
#endif
}

Sphere SphereCollider::BuildSphere() const
{
	Transform* transform = GetTransform();
	return Sphere
	{
		Offset + transform->Position,
		Radius
	};
}

OBB SphereCollider::GetBounds() const
{
	Transform* transform = GetTransform();
	return OBB
	{
		transform->Position + Offset,
		vec3 { Radius, Radius, Radius },
		mat4(1.0f) // Rotation
	};
};
bool SphereCollider::LineTest(Line& line) { return BuildSphere().LineTest(line); }
bool SphereCollider::IsPointInside(glm::vec3 point) const { return BuildSphere().IsPointInside(point); }
bool SphereCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildSphere().Raycast(ray, outResult); }

// https://scienceworld.wolfram.com/physics/MomentofInertiaSphere.html
mat4 SphereCollider::InverseTensor()
{
	Rigidbody* rb = GetRigidbody();
	float mass = rb->GetMass();
	if (!rb || mass == 0.0f)
		return mat4(0.0f);

	float r2 = Radius * Radius;
	float fraction = (2.0f / 5.0f);
	float i = r2 * mass * fraction;

	return inverse(mat4(
		i, 0, 0, 0,
		0, i, 0, 0,
		0, 0, i, 0,
		0, 0, 0, i
	));
}

bool SphereCollider::CheckCollision(const Collider* other) const { return other->CheckCollision(this); }

bool SphereCollider::CheckCollision(const BoxCollider* other) const
{
	return TestSphereBoxCollider(
		BuildSphere(),
		other->GetOBB()
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