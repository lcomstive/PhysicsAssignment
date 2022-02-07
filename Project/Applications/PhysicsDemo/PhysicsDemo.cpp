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
	CurrentScene()->GetPhysics().SetGravity({ 0, -10, 0 });

	// Default Cube Mesh //
	MeshRenderer::MeshInfo meshInfo;
	meshInfo.Mesh = Mesh::Cube();
	meshInfo.Material = {};

	CreateWalls();

	// Main Camera //
	GameObject* cameraObj = new GameObject(CurrentScene(), "Main Camera");
	cameraObj->AddComponent<Camera>();
	cameraObj->GetTransform()->Position = { 0, 0, 20 };
	cameraObj->GetTransform()->Rotation = { 0, radians(-90.0f), 0 }; // From euler angles

	meshInfo.Material.Albedo = { 1, 0, 0, 1 };

	const int TowerSize = 5;
	for (int x = 0; x < TowerSize; x++)
	{
		for (int y = 0; y < TowerSize; y++)
		{
			for (int z = 0; z < TowerSize; z++)
			{
				GameObject* go = new GameObject(CurrentScene(), "Tower " + to_string(y));
				go->GetTransform()->Position.x = x * 5;
				go->GetTransform()->Position.y = y * 5;
				go->GetTransform()->Position.z = z * 5;
				go->GetTransform()->Rotation = { Random(-90, 90), Random(-90, 90), Random(-90, 90) };

				Rigidbody* rb = go->AddComponent<Rigidbody>();
				// rb->CanSleep = false;
				rb->SetRestitution(0.5f);

				bool sphere = false; // rand() % 2 == 0;
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
	}
}

void PhysicsDemo::OnShutdown()
{
	// Test removing components
	CurrentScene()->Root()->GetTransform()->GetChildren()[0]->GetGameObject()->RemoveComponent<MeshRenderer>();

	delete defaultShader;
}

RaycastHit Hit = {};
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


	Ray ray;
	ray.Origin = Camera::GetMainCamera()->GetTransform()->Position;
	ray.Direction = { 0, 0, -1 };

	Collider* hitCollider = CurrentScene()->GetPhysics().Raycast(ray, &Hit);

	if (hitCollider && Input::IsKeyPressed(GLFW_KEY_F))
	{
		Rigidbody* rb = hitCollider->GetRigidbody();
		if (rb)
			rb->ApplyForce(-Hit.Normal * 10.0f, ForceMode::Impulse);
	}
}

void PhysicsDemo::OnDraw()
{
	ImGui::Text("FPS: %f\n", Renderer::GetFPS());
	ImGui::Text("Render  Frame Time: %fms", Renderer::GetDeltaTime() * 1000.0f);
	ImGui::Text("Physics Frame Time: %fms", (CurrentScene()->GetPhysics().LastTimeStep().count()) * 1.0f);
	ImGui::Text("Resolution: (%d, %d)", Renderer::GetResolution().x, Renderer::GetResolution().y);
	ImGui::Text("VSync: %s", Renderer::GetVSync() ? "Enabled" : "Disabled");
	ImGui::Text("Samples: %d", Renderer::GetSamples());

	if (CurrentScene()->GetPhysics().GetState() == PhysicsPlayState::Paused)
		ImGui::Text("PHYSICS PAUSED");
}

void PhysicsDemo::OnDrawGizmos()
{
	if (Hit.Distance > 0)
	{
		Gizmos::Colour = { 0, 1, 1, 1 };
		Gizmos::DrawCube(Hit.Point, { 0.1f, 0.1f, 0.1f });
	}
}

const float WallDistance = 100.0f, WallThickness = 25;
const vec3 WallOffset = { 0, WallDistance - WallThickness - 5.0f, 0 };
void PhysicsDemo::CreateWall(vec3 axis)
{
	GameObject* wall = new GameObject(CurrentScene(), "Wall");
	// wall->AddComponent<MeshRenderer>()->Meshes = { MeshRenderer::MeshInfo { Mesh::Cube(), Material {}}};
	wall->AddComponent<BoxCollider>();

	Transform* transform = wall->GetTransform();
	transform->Scale = vec3(WallDistance) - abs(axis) * vec3(WallDistance - WallThickness);
	transform->Rotation = axis * radians(90.0f); // Rotate 90 degrees on axis
	transform->Position = -axis * WallDistance + WallOffset;
}

void PhysicsDemo::CreateWalls()
{
	CreateWall(vec3 {  0,  1,  0 });
	CreateWall(vec3 {  0, -1,  0 });
	CreateWall(vec3 {  1,  0,  0 });
	CreateWall(vec3 { -1,  0,  0 });
	CreateWall(vec3 {  0,  0,  1 });
	CreateWall(vec3 {  0,  0, -1 });
}
