#include <Engine/GameObject.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace Engine;
using namespace Engine::Components;

GameObject* Component::GetGameObject() const { return m_GameObject; }
Transform*  Component::GetTransform()  const { return m_GameObject->GetTransform(); }
Rigidbody* Component::GetRigidbody() const { return m_GameObject->GetComponent<Rigidbody>(); }