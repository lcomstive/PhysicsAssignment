#pragma once
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Physics/CollisionInfo.hpp>

namespace Engine::Components
{
	struct BoxCollider;
	struct PlaneCollider;
	struct SphereCollider;

	/// <summary>
	/// Abstract class for colliders
	/// </summary>
	struct Collider : public Component
	{
		virtual Physics::OBB GetBounds() const = 0;
		virtual bool IsPointInside(glm::vec3 point) const = 0;
		virtual bool Raycast(Physics::Ray& ray, Physics::RaycastHit* outResult = nullptr) = 0;

		virtual Physics::Collision CheckCollision(const Collider* other) const = 0;
		virtual Physics::Collision CheckCollision(const BoxCollider* other) const = 0;
		virtual Physics::Collision CheckCollision(const PlaneCollider* other) const = 0;
		virtual Physics::Collision CheckCollision(const SphereCollider* other) const = 0;

	protected:
		virtual void Added() override;
		virtual void Removed() override;
	};
}