#include <glm/glm.hpp>
#include <Engine/Physics/Octree.hpp>

using namespace std;
using namespace glm;
using namespace Engine::Physics;
using namespace Engine::Components;

void Engine::Physics::SplitTree(OctreeNode* node, int depth)
{
	if (!node || depth-- <= 0)
		return;

	if (!node->Children)
	{
		// Create children
		node->Children = new OctreeNode[8];

		// Position
		vec3 p = node->Bounds.Position;
		// Half extents
		vec3 e = node->Bounds.Extents / 2.0f;

		node->Children[0].Bounds = { p + vec3(-e.x,  e.y, -e.z), e };
		node->Children[1].Bounds = { p + vec3(e.x,  e.y, -e.z), e };
		node->Children[2].Bounds = { p + vec3(-e.x,  e.y,  e.z), e };
		node->Children[3].Bounds = { p + vec3(e.x,  e.y,  e.z), e };
		node->Children[4].Bounds = { p + vec3(-e.x, -e.y, -e.z), e };
		node->Children[5].Bounds = { p + vec3(e.x, -e.y, -e.z), e };
		node->Children[6].Bounds = { p + vec3(-e.x, -e.y,  e.z), e };
		node->Children[7].Bounds = { p + vec3(e.x, -e.y,  e.z), e };
	}

	if (!node->Children || node->Colliders.empty())
		return;

	for (uint32_t i = 0; i < 8; i++)
	{
		for (uint32_t j = 0; j < (uint32_t)node->Colliders.size(); j++)
		{
			OBB bounds = node->Colliders[j]->GetBounds();
			if (TestBoxBoxCollider(node->Children[i].Bounds, bounds))
				node->Children[i].Colliders.emplace_back(node->Colliders[j]);
		}
	}

	node->Colliders.clear();

	for (int i = 0; i < 8; i++)
		SplitTree(&node->Children[i], depth);
}

void Engine::Physics::Insert(OctreeNode* node, Collider* collider)
{
	OBB bounds = collider->GetBounds();
	if (!TestBoxBoxCollider(node->Bounds, bounds))
		return;
	if (!node->Children)
		node->Colliders.emplace_back(collider);
	else
		for (int i = 0; i < 8; i++)
			Insert(&node->Children[i], collider);
}

void Engine::Physics::Remove(OctreeNode* node, Collider* collider)
{
	if (node->Children)
	{
		for (int i = 0; i < 8; i++)
			Remove(&node->Children[i], collider);
		return;
	}

	const auto& it = find(node->Colliders.begin(), node->Colliders.end(), collider);
	if (it != node->Colliders.end())
		node->Colliders.erase(it);
}

void Engine::Physics::Update(OctreeNode* node, Collider* collider)
{
	Remove(node, collider);
	Insert(node, collider);
}