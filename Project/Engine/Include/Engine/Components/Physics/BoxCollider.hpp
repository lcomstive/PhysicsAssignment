#pragma once
#include <Engine/Components/Physics/Collider.hpp>

namespace Engine::Components
{
	struct BoxCollider : public Collider
	{
		/// <summary>
		/// Half collider size, relative to attached object's scale
		/// </summary>
		glm::vec3 Extents = { 0.5f, 0.5f, 0.5f };

		/// <summary>
		/// Position offset relative to attached object
		/// </summary>
		glm::vec3 Offset = { 0, 0, 0 };

		Physics::OBB BuildOBB() const;

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