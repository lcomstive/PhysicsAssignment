#pragma once
#include <Engine/Components/Physics/Collider.hpp>

namespace Engine::Components
{
	struct BoxCollider : public Collider
	{
		/// <summary>
		/// Collider size, relative to attached object's scale
		/// </summary>
		glm::vec3 Extents = { 1.0f, 1.0f, 1.0f };

		/// <summary>
		/// Position offset relative to attached object
		/// </summary>
		glm::vec3 Offset = { 0, 0, 0 };

		Physics::OBB BuildOBB() const;

		void DrawGizmos() override;

		Physics::OBB GetBounds() const override;
		bool LineTest(Physics::Line& line) override;
		bool IsPointInside(glm::vec3 point) const override;
		bool Raycast(Physics::Ray& ray, Physics::RaycastHit* outResult) override;

		bool CheckCollision(const Collider* other) const override;
		bool CheckCollision(const BoxCollider* other) const override;
		bool CheckCollision(const PlaneCollider* other) const override;
		bool CheckCollision(const SphereCollider* other) const override;
	};
}
