#include <imgui.h>
#include "PhysicsDemo.hpp"
#include <Engine/Utilities.hpp>
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Components/Camera.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Components/Physics/Particle.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>
#include <Engine/Components/Graphics/MeshRenderer.hpp>

#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/Physics/PlaneCollider.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>

#pragma warning(disable : 4244)

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

PhysicsDemo::PhysicsDemo(ApplicationArgs args) : Application(args) { }

Shader* defaultShader = nullptr;

void PhysicsDemo::OnStart()
{
	vec3 center = { 0, 0, 0 };
	CurrentScene()->GetPhysics().Accelerate(center, 1000.0f, 10);
	CurrentScene()->GetPhysics().SetGravity({ 0, -1, 0 });

	// Default Cube Mesh //
	MeshRenderer::MeshInfo meshInfo;
	meshInfo.Mesh = Mesh::Cube();
	meshInfo.Material = {};

	// Floor
	GameObject* floor = new GameObject(CurrentScene(), "Floor");
	// floor->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	floor->AddComponent<BoxCollider>();
	// floor->AddComponent<SphereCollider>();
	floor->GetTransform()->Position.y = -3.0f;
	floor->GetTransform()->Scale = { 50, 0.5f, 50 };

	// Main Camera //
	GameObject* cameraObj = new GameObject(CurrentScene(), "Main Camera");
	cameraObj->AddComponent<Camera>();
	cameraObj->GetTransform()->Position = { 0, 0, 20 };
	cameraObj->GetTransform()->Rotation = { 0, radians(-90.0f), 0 }; // From euler angles

	meshInfo.Material.Albedo = { 1, 0, 0, 1 };

	const int TowerSize = 3;
	for (int y = 0; y < TowerSize; y++)
	{
		GameObject* go = new GameObject(CurrentScene(), "Tower " + to_string(y));
		go->GetTransform()->Position.y = y * 2.5f;

		go->AddComponent<Rigidbody>()->SetRestitution(1.0f);

		bool sphere = rand() % 2 == 0;
		if (sphere)
		{
			meshInfo.Mesh = Mesh::Sphere();
			go->AddComponent<SphereCollider>();
		}
		else
		{
			meshInfo.Mesh = Mesh::Cube();
			go->AddComponent<BoxCollider>();
		}
		go->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	}
}

void PhysicsDemo::OnShutdown()
{
	// Test removing components
	CurrentScene()->Root()->GetTransform()->GetChildren()[0]->GetGameObject()->RemoveComponent<MeshRenderer>();

	delete defaultShader;
}

const float CameraSpeed = 5.0f;
const float CameraRotationSpeed = 5.0f;
void PhysicsDemo::OnUpdate()
{
	// First person view camera controls
	Camera* camera = Camera::GetMainCamera();
	Transform* transform = camera->GetTransform();

	float speed = CameraSpeed * (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT) ? 2.0f : 1.0f);

	if (Input::IsKeyDown(GLFW_KEY_A)) transform->Position.x -= speed * Renderer::GetDeltaTime();
	if (Input::IsKeyDown(GLFW_KEY_D)) transform->Position.x += speed * Renderer::GetDeltaTime();
	if (Input::IsKeyDown(GLFW_KEY_W)) transform->Position.z -= speed * Renderer::GetDeltaTime();
	if (Input::IsKeyDown(GLFW_KEY_S)) transform->Position.z += speed * Renderer::GetDeltaTime();

	if (Input::IsKeyDown(GLFW_KEY_Q)) transform->Position.y -= speed * Renderer::GetDeltaTime();
	if (Input::IsKeyDown(GLFW_KEY_E)) transform->Position.y += speed * Renderer::GetDeltaTime();

	camera->FieldOfView = std::clamp(camera->FieldOfView - Input::GetScrollDelta(), 10.0f, 120.0f);

	Input::ShowMouse(!Input::IsMouseDown(GLFW_MOUSE_BUTTON_2));
	if (Input::IsMouseDown(GLFW_MOUSE_BUTTON_2))
	{
		vec2 mouseDelta = Input::GetMouseDelta();
		transform->Rotation += radians(vec3{ -mouseDelta.y, mouseDelta.x, 0 } *float(CameraRotationSpeed * Renderer::GetDeltaTime()));
	}

	if (Input::IsKeyPressed(GLFW_KEY_F11))
		ToggleFullscreen();

	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Exit();

	if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		CurrentScene()->GetPhysics().TogglePause();

	if (Input::IsKeyPressed(GLFW_KEY_J))
	{
		Log::Debug("OWO");
	}
}

void PhysicsDemo::OnDraw()
{
	ImGui::Text("FPS: %f\n", Renderer::GetFPS());
	ImGui::Text("Frame Time: %f", Renderer::GetDeltaTime());
	ImGui::Text("Resolution: (%d, %d)", Renderer::GetResolution().x, Renderer::GetResolution().y);
	ImGui::Text("VSync: %s", Renderer::GetVSync() ? "Enabled" : "Disabled");
	ImGui::Text("Samples: %d", Renderer::GetSamples());

	if (CurrentScene()->GetPhysics().GetState() == PhysicsPlayState::Paused)
		ImGui::Text("PHYSICS PAUSED");
}

void PhysicsDemo::OnDrawGizmos()
{
	Transform* cameraTransform = Camera::GetMainCamera()->GetTransform();

	Ray ray;
	ray.Origin = cameraTransform->Position;
	ray.Direction = { 0, 0, -1 };

	RaycastHit hit;
	Collider* hitCollider = CurrentScene()->GetPhysics().Raycast(ray, &hit);
	if (hitCollider)
	{
		Gizmos::Colour = { 0, 1, 1, 1 };
		Gizmos::DrawCube(hit.Point, { 0.1f, 0.1f, 0.1f });
	}
}
