#pragma once
#include <vector>
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Components/Physics/Collider.hpp>

namespace Engine::Physics
{
	struct OctreeNode
	{
		AABB Bounds;
		OctreeNode* Children;
		std::vector<Components::Collider*> Colliders;

		OctreeNode();
		~OctreeNode();

		void Split(int depth);

		void Insert(Components::Collider* collider);
		void Remove(Components::Collider* collider);
		void Update(Components::Collider* collider);
	};
}