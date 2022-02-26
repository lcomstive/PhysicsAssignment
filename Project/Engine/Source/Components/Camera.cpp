#include <glm/glm.hpp>
#include <Engine/GameObject.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Engine/Components/Camera.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Engine/Components/Transform.hpp>

using namespace glm;
using namespace Engine;
using namespace Engine::Graphics;
using namespace Engine::Components;

Camera* Camera::s_MainCamera = nullptr;

const vec3 WorldUp = { 0, 1, 0 };

Camera::Camera() :
	m_Up(0, 1, 0),
	m_Right(1, 0, 0),
	m_Forward(0, 0, 1),
	m_ViewMatrix(1.0f),
	m_GlobalPosition(0),
	m_ProjectionMatrix(1.0f)
{
	if (!s_MainCamera)
		SetMainCamera();
}

void Camera::SetMainCamera() { s_MainCamera = this; }
Camera* Camera::GetMainCamera() { return s_MainCamera; }

mat4 Camera::GetViewMatrix() { return m_ViewMatrix; }
mat4 Camera::GetProjectionMatrix() { return m_ProjectionMatrix; }

void Camera::FillShader(Shader* shader)
{
	shader->Set("camera.Position", m_GlobalPosition);
	shader->Set("camera.ViewMatrix", GetViewMatrix());
	shader->Set("camera.ProjectionMatrix", GetProjectionMatrix());
}

vec3 Camera::GetUpDirection() { return m_Up; }
vec3 Camera::GetRightDirection() { return m_Right; }
vec3 Camera::GetForwardDirection() { return m_Forward; }

void Camera::Removed()
{
	Component::Removed();
	if (s_MainCamera == this)
		s_MainCamera = nullptr;
}

void Camera::Update(float deltaTime)
{
	Transform* transform = GetTransform();

	m_GlobalPosition = transform->GetGlobalPosition();

	m_Forward = normalize(vec3
		{
			cos(transform->Rotation.y) * cos(transform->Rotation.x),
			sin(transform->Rotation.x),
			sin(transform->Rotation.y) * cos(transform->Rotation.x)
		});

	m_Right = normalize(cross(m_Forward, WorldUp));
	m_Up = normalize(cross(m_Right, m_Forward));

	// Create view matrix
	m_ViewMatrix = lookAt(transform->Position, transform->Position + m_Forward, m_Up);

	// Create projection matrix
	ivec2 res = Renderer::GetResolution();
	res.x = max(res.x, 1);
	res.y = max(res.y, 1);
	float aspectRatio = res.x / (float)res.y;
	if (Orthographic)
	{
		float width = OrthoSize * aspectRatio;
		float height = OrthoSize;

		m_ProjectionMatrix = ortho(-width, width, -height, height, ClipNear, ClipFar);
	}
	else
		m_ProjectionMatrix = perspective(radians(FieldOfView), aspectRatio, ClipNear, ClipFar);
}