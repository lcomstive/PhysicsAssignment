#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/Physics/Collider.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Components;

void Collider::Added() { GetGameObject()->GetScene()->GetPhysics().AddCollider(this); }
void Collider::Removed() { GetGameObject()->GetScene()->GetPhysics().RemoveCollider(this); }

mat4& Collider::InverseTensor() { return m_InverseTensor; }