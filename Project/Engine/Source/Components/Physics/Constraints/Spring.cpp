#include <Engine/Utilities.hpp> // Magnitude
#include <Engine/Components/Transform.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>
#include <Engine/Components/Physics/Constraints/Spring.hpp>

using namespace glm;
using namespace Engine::Components;

Particle* Spring::GetPoint1() { return m_Point1; }
Particle* Spring::GetPoint2() { return m_Point2; }

void Spring::SetBodies(Particle* a, Particle* b)
{
	m_Point1 = a;
	m_Point2 = b;
}

void Spring::SetConstants(float stiffness, float dampening)
{
	m_Stiffness = std::max(stiffness, 0.0f);
	m_Dampening = std::clamp(dampening, 0.0f, 1.0f);
}

float Spring::GetRestingLength() { return m_RestingLength; }
void  Spring::SetRestingLength(float length) { m_RestingLength = length; }

void Spring::ApplyForces(float timestep)
{
	vec3 relPos = m_Point1->GetTransform()->Position - m_Point2->GetTransform()->Position;
	vec3 relVel = m_Point1->GetVelocity() - m_Point2->GetVelocity();

	// Hooke's Law
	float x = Magnitude(relPos) - m_RestingLength;
	float v = Magnitude(relVel);

	// Restoring force of spring
	float F = -m_Stiffness * x - m_Dampening * v;

	// Turn it into an impulse force
	vec3 impulse = normalize(relPos) * F;

	m_Point1->ApplyForce( impulse * m_Point1->InverseMass(), ForceMode::Impulse);
	m_Point2->ApplyForce(-impulse * m_Point2->InverseMass(), ForceMode::Impulse);
}