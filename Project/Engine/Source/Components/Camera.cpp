#include <glm/glm.hpp>
#include <Engine/GameObject.hpp>
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

Camera::Camera()
{
	if (!s_MainCamera)
		SetMainCamera();
}

void Camera::SetMainCamera() { s_MainCamera = this; }
Camera* Camera::GetMainCamera() { return s_MainCamera; }

mat4 Camera::GetViewMatrix()
{
	Transform* transform = GetTransform();
	vec3 forward =
	{
		cos(transform->Rotation.y) * cos(transform->Rotation.x),
		sin(transform->Rotation.x),
		sin(transform->Rotation.y) * cos(transform->Rotation.x)
	};
	forward = normalize(forward);

	vec3 right = normalize(cross(forward, WorldUp));
	vec3 up = normalize(cross(right, forward));

	return lookAt(transform->Position, transform->Position + forward, up);
}

mat4 Camera::GetProjectionMatrix()
{
	ivec2 res = Renderer::GetResolution();
	res.x = max(res.x, 1);
	res.y = max(res.y, 1);
	float aspectRatio = res.x / (float)res.y;
	if (Orthographic)
	{
		float width = OrthoSize * aspectRatio;
		float height = OrthoSize;

		return ortho(-width, width, -height, height, ClipNear, ClipFar);
	}
	else
		return perspective(radians(FieldOfView), aspectRatio, ClipNear, ClipFar);
}

void Camera::FillShader(Shader* shader)
{
	shader->Set("camera.ViewMatrix", GetViewMatrix());
	shader->Set("camera.ProjectionMatrix", GetProjectionMatrix());
	shader->Set("camera.Position", GetTransform()->GetGlobalPosition());
}