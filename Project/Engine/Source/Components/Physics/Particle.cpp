#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>
#include <Engine/Components/Physics/Particle.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>

using namespace glm;
using namespace std;
using namespace Engine::Physics;
using namespace Engine::Components;

Particle::Particle() :
	m_Force(),
	m_Mass(1.0f),
	IsTrigger(false),
	UseGravity(true),
	m_Friction(0.95f),
	m_Bounciness(0.7f),
	m_PreviousPosition(),
	m_Initialised(false)
{ }

PhysicsSystem& Particle::GetSystem() { return GetGameObject()->GetScene()->GetPhysics(); }

void Particle::ApplyForce(glm::vec3 force)
{
	m_Force += force;
}

float Particle::GetMass() { return m_Mass; }
void  Particle::SetMass(float mass) { m_Mass = std::clamp(mass, 0.0001f, FLT_MAX); }
float Particle::GetBounciness() { return m_Bounciness; }
void  Particle::SetBounciness(float value) { m_Bounciness = value; }
float Particle::GetFriction() { return m_Friction; }
void  Particle::SetFriction(float value) { m_Friction = value; }

void Particle::Added()
{
	GetGameObject()->AddComponent<SphereCollider>();
}

void Particle::FixedUpdate(float timestep)
{
	Transform* transform = GetTransform();

	if (!m_Initialised)
	{
		m_PreviousPosition = transform->Position;
		m_Initialised = true;
	}

	vec3 velocity = transform->Position - m_PreviousPosition;
	m_PreviousPosition = transform->Position;
	float deltaSquare = timestep * timestep;

	// Apply gravity
	m_Force += m_Mass * GetSystem().GetGravity();

	transform->Position += velocity * m_Friction + m_Force * deltaSquare;

	SolveConstraints(timestep);

	m_Force = { 0, 0, 0 }; // Reset applied force
}

void Particle::SolveConstraints(float timestep)
{
	Transform* transform = GetTransform();
	Collider* rbCollider = GetGameObject()->GetComponent<Collider>(true);
	PhysicsSystem& system = GetSystem();

	Line line = { m_PreviousPosition, transform->Position };
	if (!system.LineTest(line, rbCollider))
		return; // No collisions will occur

	vec3 velocity = transform->Position - m_PreviousPosition;
	vec3 direction = normalize(velocity);

	Ray ray;
	ray.Origin = m_PreviousPosition;
	ray.Direction = direction;

	RaycastHit hit;
	Collider* hitCollider = system.Raycast(ray, rbCollider, &hit);
	if(hitCollider)
	{
		transform->Position = hit.Point + hit.Normal * 0.003f;

		vec3 vn = hit.Normal * dot(hit.Normal, velocity);
		vec3 vt = velocity - vn;

		// Record current position to avoid tunnelling
		m_PreviousPosition = transform->Position - (vt - vn * m_Bounciness);

		Log::Debug("Hit '" + hitCollider->GetGameObject()->GetName() + "'");
	}
}