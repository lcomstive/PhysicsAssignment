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
		virtual Physics::OBB& GetBounds() = 0;
		virtual bool LineTest(Physics::Line& line) = 0;
		virtual bool IsPointInside(glm::vec3& point) const = 0;
		virtual bool Raycast(Physics::Ray& ray, Physics::RaycastHit* outResult = nullptr) = 0;

		virtual bool CheckCollision(Collider* other) = 0;
		virtual bool CheckCollision(BoxCollider* other) = 0;
		virtual bool CheckCollision(PlaneCollider* other) = 0;
		virtual bool CheckCollision(SphereCollider* other) = 0;

		virtual glm::mat4& InverseTensor();

	protected:
		virtual void Added() override;
		virtual void Removed() override;

	private:
		glm::mat4 m_InverseTensor = glm::mat4(0.0f);
	};
}