#pragma once
#include <Engine/Components/Physics/Collider.hpp>

namespace Engine::Components
{
	struct SphereCollider : public Collider
	{
		float Radius	 = 1.0f;
		glm::vec3 Offset = { 0, 0, 0 }; // Relative to attached GameObject's transform
		
		Physics::Sphere BuildSphere() const;

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