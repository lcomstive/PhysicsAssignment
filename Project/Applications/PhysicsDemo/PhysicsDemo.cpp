#include <imgui.h>
#include "PhysicsDemo.hpp"
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Components/Camera.hpp>
#include <Engine/Graphics/Renderer.hpp>
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
	// Default Cube Mesh //
	MeshRenderer::MeshInfo meshInfo;
	meshInfo.Mesh = Mesh::Cube();
	meshInfo.Material = {};

	// Floor
	GameObject* floor = new GameObject(CurrentScene(), "Floor");
	floor->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	floor->AddComponent<BoxCollider>();
	floor->GetTransform()->Position.y = -3.0f;
	floor->GetTransform()->Scale = { 10, 0.5f, 10 };

	// Main Camera //
	GameObject* cameraObj = new GameObject(CurrentScene(), "Main Camera");
	Camera* camera = cameraObj->AddComponent<Camera>();
	cameraObj->GetTransform()->Position = { 0, 0, 10 };
	cameraObj->GetTransform()->Rotation = { 0, radians(-90.0f), 0 }; // From euler angles

	// Cube
	GameObject* physicsObj = new GameObject(CurrentScene(), "Cube");
	// physicsObj->AddComponent<Rigidbody>();
	physicsObj->AddComponent<BoxCollider>();
	physicsObj->AddComponent<MeshRenderer>()->Meshes = { meshInfo };

	// Sphere
	physicsObj = new GameObject(CurrentScene(), "Sphere");
	physicsObj->AddComponent<SphereCollider>();
	meshInfo.Mesh = Mesh::Sphere();
	physicsObj->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	
	physicsObj->GetTransform()->Position = { 2, 2, 0 };
	
	// Plane
	physicsObj = new GameObject(CurrentScene(), "Plane");

	PlaneCollider* planeCollider = physicsObj->AddComponent<PlaneCollider>();
	planeCollider->Normal = normalize(vec3{ 0.5f, 0, 0.5f });
	planeCollider->Distance = -10.0f;

	const int GridSize = 15;
	for (int x = -GridSize; x < GridSize; x++)
	{
		for (int y = -GridSize; y < GridSize; y++)
		{
			GameObject* go = new GameObject(CurrentScene(), "GameObject " + to_string(x) + "," + to_string(y));
			go->GetTransform()->Position = { x * 2.5f, y * 2.5f, -15 };

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
		transform->Rotation += radians(vec3{ -mouseDelta.y, mouseDelta.x, 0 } * float(CameraRotationSpeed * Renderer::GetDeltaTime()));
	}

	// Toggle global wireframe mode with `P` key
	if (Input::IsKeyPressed(GLFW_KEY_P))
		Renderer::SetWireframe(!Renderer::GetWireframeMode());

	if (Input::IsKeyPressed(GLFW_KEY_F11))
		ToggleFullscreen();

	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Exit();
}


void PhysicsDemo::OnDraw()
{
	ImGui::Text("FPS: %f\n", Renderer::GetFPS());
	ImGui::Text("Frame Time: %f", Renderer::GetDeltaTime());
	ImGui::Text("Resolution: (%d, %d)", Renderer::GetResolution().x, Renderer::GetResolution().y);
	ImGui::Text("VSync: %s", Renderer::GetVSync() ? "Enabled" : "Disabled");
	ImGui::Text("Samples: %d", Renderer::GetSamples());
}

void PhysicsDemo::OnDrawGizmos()
{
	Transform* cameraTransform = Camera::GetMainCamera()->GetTransform();

	Ray ray;
	ray.Origin = cameraTransform->Position;
	ray.Direction = normalize(cameraTransform->Rotation);

	RaycastHit hit;
	Collider* hitCollider = CurrentScene()->GetPhysics().Raycast(ray, &hit);
	if (hitCollider)
	{
		Gizmos::Colour = { 0, 1, 1, 1 };
		Gizmos::DrawCube(hit.Point, { 0.1f, 0.1f, 0.1f });
	}
}