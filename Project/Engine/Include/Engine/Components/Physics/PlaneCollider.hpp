#pragma once
#include <Engine/Components/Physics/Collider.hpp>

namespace Engine::Components
{
	struct PlaneCollider : public Collider
	{
		float Distance	 = 0.0f; // Distance along normal
		glm::vec3 Normal = { 0, 1, 0 };

		Physics::Plane BuildPlane() const;

		void DrawGizmos() override;

		Physics::OBB GetBounds() const override;
		bool IsPointInside(glm::vec3 point) const override;
		bool Raycast(Physics::Ray& ray, Physics::RaycastHit* outResult) override;

		Physics::Collision CheckCollision(const Collider* other) const override;
		Physics::Collision CheckCollision(const BoxCollider* other) const override;
		Physics::Collision CheckCollision(const PlaneCollider* other) const override;
		Physics::Collision CheckCollision(const SphereCollider* other) const override;
	};
}