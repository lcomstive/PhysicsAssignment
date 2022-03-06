#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>

using namespace std;
using namespace Engine;
using namespace Engine::Components;

GameObject::GameObject(Scene* scene, string name) : m_Scene(scene), m_Name(name)
{
	m_Transform = AddComponent<Transform>();
	if(m_Scene)
		m_Transform->SetParent(m_Scene->Root().GetTransform());
}

GameObject::GameObject(Transform* parent, string name) : GameObject(parent->GetGameObject(), name) { }
GameObject::GameObject(GameObject* parent, string name) : m_Name(name), m_Components(), m_Scene(parent->m_Scene)
{
	m_Transform = AddComponent<Transform>();

	// Update transform hierarchy
	if(parent && parent->m_Transform)
		m_Transform->SetParent(parent->m_Transform);
}

GameObject::~GameObject()
{	
	// Delete and remove attached components
	for (auto& pair : m_Components)
	{
		pair.second->Removed();
		delete pair.second;
	}
	m_Transform = nullptr;
	m_Components.clear();
}

string GameObject::GetName() { return m_Name; }
Scene* GameObject::GetScene() { return m_Scene; }
void GameObject::SetName(string name) { m_Name = name; }
Transform* GameObject::GetTransform() { return m_Transform; }

// The below functions call a particular function on all attached components,
// then recursively call on all child objects

#define COMPONENT_CALL(fnName, paramType, param) \
								void GameObject::fnName(paramType param) { \
									for(auto& pair : m_Components) pair.second->fnName(param); \
									for(Transform* child : m_Transform->GetChildren()) child->GetGameObject()->fnName(param); \
								}

COMPONENT_CALL(Draw)
COMPONENT_CALL(DrawGizmos)
COMPONENT_CALL(Update, float, deltaTime)