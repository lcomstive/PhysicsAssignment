#include <Engine/Scene.hpp>
#include <Engine/Utilities.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Physics/PhysicsSystem.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>

using namespace glm;
using namespace std;
using namespace Engine;
using namespace Engine::Physics;
using namespace Engine::Components;

Rigidbody::Rigidbody() :
	m_Force(),
	m_CoR(0.5f),
	m_Mass(1.0f),
	m_Velocity(),
	IsTrigger(false),
	UseGravity(true),
	m_Friction(0.6f),
	m_PreviousPosition(),
	m_Initialised(false)
{ }

PhysicsSystem& Rigidbody::GetSystem() { return GetGameObject()->GetScene()->GetPhysics(); }
float Rigidbody::GetMass() { return m_Mass; }
void  Rigidbody::SetMass(float mass) { m_Mass = std::clamp(mass, 0.0f, FLT_MAX); }
float Rigidbody::GetRestitution() { return m_CoR; }
void  Rigidbody::SetRestitution(float value) { m_CoR = value; }
float Rigidbody::GetFriction() { return m_Friction; }
void  Rigidbody::SetFriction(float value) { m_Friction = value; }

float Rigidbody::InverseMass()
{
	return m_Mass <= 0.0f ? 0.0f : (1.0f / m_Mass);
}

void Rigidbody::ApplyForce(glm::vec3 force, ForceMode mode)
{
	switch (mode)
	{
	default:
	case ForceMode::Acceleration:
		m_Force += force;
		break;
	case ForceMode::Impulse:
		m_Velocity += force;
		break;
	}
}

void Rigidbody::ApplyWorldForces()
{
	ApplyForce(GetSystem().GetGravity() * m_Mass);
}

void Rigidbody::FixedUpdate(float timestep)
{
	Transform* transform = GetTransform();

	if (!m_Initialised)
	{
		m_PreviousPosition = transform->Position;
		m_Initialised = true;
	}

	/*
	vec3 velocity = transform->Position - m_PreviousPosition;
	m_PreviousPosition = transform->Position;
	float deltaSquare = timestep * timestep;

	// Apply gravity
	m_Force += m_Mass * GetSystem().GetGravity();

	transform->Position += velocity + m_Force * deltaSquare;

	SolveConstraints(timestep);

	m_Force = { 0, 0, 0 }; // Reset applied force
	*/

	const float damping = 0.98f;
	vec3 acceleration = m_Force * InverseMass();
	m_Velocity += (acceleration * timestep) * damping;

	transform->Position += m_Velocity * timestep;
}

// Linear impulse
void Rigidbody::ApplyImpulse(Rigidbody* other, CollisionManifold manifold, int contactIndex)
{
	float invMass1 = InverseMass();
	float invMass2 = other->InverseMass();
	float invMassSum = invMass1 + invMass2;
	if (BasicallyZero(invMassSum))
		return;

	vec3 relativeVel = other->m_Velocity - m_Velocity;
	vec3 relativeNorm = normalize(manifold.Normal);

	if (dot(relativeVel, relativeNorm) > 0)
		return; // Moving away from each other? Ignore

	float e = fminf(m_CoR, other->m_CoR);
	float numerator = (-(1.0f + e) * dot(relativeVel, relativeNorm));
	float j = numerator / invMassSum; // Magnitude of impulse to apply
	if (!manifold.Contacts.empty() && j != 0.0f)
		j /= (float)manifold.Contacts.size();

	vec3 impulse = relativeNorm * j;
	m_Velocity		  -= impulse * invMass1;
	other->m_Velocity += impulse * invMass2;

	// Tangent to normal, used to apply friction
	vec3 t = relativeVel - (relativeNorm * dot(relativeVel, relativeNorm));
	if (BasicallyZero(MagnitudeSqr(t)))
		return;
	t = normalize(t);

	// Get magnitude of friction to apply
	numerator = -dot(relativeVel, t);
	float jt = numerator / invMassSum;
	if (!manifold.Contacts.empty() && jt != 0.0f)
		jt /= (float)manifold.Contacts.size();
	if (BasicallyZero(jt))
		return;

	float friction = sqrtf(m_Friction * other->m_Friction);
	jt = std::clamp(jt, -j * friction, j * friction);  // Coulumb's Law

	vec3 tangentImpulse = t * jt;
	m_Velocity		  -= tangentImpulse * invMass1;
	other->m_Velocity += tangentImpulse * invMass2;
}

void Rigidbody::SolveConstraints(float timestep)
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
		m_PreviousPosition = transform->Position - (vt - vn * m_CoR);

		Log::Debug("Hit '" + hitCollider->GetGameObject()->GetName() + "'");
	}
}