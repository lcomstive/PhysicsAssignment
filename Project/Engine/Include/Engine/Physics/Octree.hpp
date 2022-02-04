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

		OctreeNode() : Children(nullptr) { }
		~OctreeNode()
		{
			if (Children)
				delete[] Children;
		}
	};

	void SplitTree(OctreeNode* node, int depth);

	void Insert(OctreeNode* node, Components::Collider* collider);
	void Remove(OctreeNode* node, Components::Collider* collider);
	void Update(OctreeNode* node, Components::Collider* collider);
}