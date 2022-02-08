#include <Engine/GameObject.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace Engine;
using namespace Engine::Components;

GameObject* Component::GetGameObject() const { return m_GameObject; }

Transform*  Component::GetTransform() 
{
	if (m_CachedTransform)
		return m_CachedTransform;
	else
		return (m_CachedTransform = m_GameObject->GetTransform());
}

Rigidbody* Component::GetRigidbody()
{
	if (m_CachedRB)
		return m_CachedRB;
	else
		return (m_CachedRB = m_GameObject->GetComponent<Rigidbody>());
}