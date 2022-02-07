#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/perpendicular.hpp>
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

void PlaneCollider::DrawGizmos()
{
#ifndef NDEBUG
	Gizmos::Colour = { 0, 1, 0, 1 };
	Gizmos::DrawWireQuad(Normal * Distance, { 100.0f, 100.0f }, eulerAngles(quatLookAt(Normal, { 0, 1, 0 })));
#endif
}

Plane PlaneCollider::BuildPlane() const { return Plane(Normal, Distance); }

OBB PlaneCollider::GetBounds() const
{
	return OBB
	{
		Normal * Distance,
		{ 1000, 1000, 1000 },
		lookAt({ 0, 0, 0 }, Normal, { 0, 1, 0 })
	};
}

bool PlaneCollider::LineTest(Line& line) { return BuildPlane().LineTest(line); }
bool PlaneCollider::Raycast(Ray& ray, RaycastHit* outResult) { return BuildPlane().Raycast(ray, outResult); }
bool PlaneCollider::IsPointInside(glm::vec3 point) const { return BuildPlane().CheckPoint(point) == PlaneIntersection::Intersecting; }

bool PlaneCollider::CheckCollision(const Collider* other) const { return other->CheckCollision(this); }

bool PlaneCollider::CheckCollision(const BoxCollider* other) const
{
	return TestBoxPlaneCollider(
		other->GetOBB(),
		BuildPlane()
	);
}

bool PlaneCollider::CheckCollision(const SphereCollider* other) const
{
	return TestSpherePlaneCollider(
		other->BuildSphere(),
		BuildPlane()
	);
}

bool PlaneCollider::CheckCollision(const PlaneCollider* other) const
{
	return TestPlanePlaneCollider(
		BuildPlane(),
		other->BuildPlane()
	);
}
