#include <imgui.h>
#include "SpringDemo.hpp"
#include <Engine/Utilities.hpp>
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Components/Physics/Particle.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>
#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/OrbitCameraController.hpp>
#include <Engine/Components/Graphics/MeshRenderer.hpp>
#include <Engine/Components/Physics/PlaneCollider.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>
#include <Engine/Components/Physics/Constraints/Spring.hpp>

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

SpringDemo::SpringDemo(ApplicationArgs args) : Application(args) { }

// Grid
Mesh* gridMesh = nullptr;
const int GridSize = 250;

Spring* DemoSpring = nullptr;

float SpringStiffness = 10.0f;
float SpringRestingLength = 1.0f;
float SpringDampingFactor = 0.5f;

GameObject* PointA = nullptr;
GameObject* PointB = nullptr;

unsigned int TotalObjects = 0;
void SpringDemo::OnStart()
{
	// Set ImGui Font
	ImGui::GetIO().Fonts->AddFontFromFileTTF("./Assets/Fonts/Source Sans Pro/SourceSansPro-Regular.ttf", 16.0f);
	
	// Main Camera //
	GameObject* cameraObj = new GameObject(CurrentScene(), "Main Camera");
	cameraObj->AddComponent<OrbitCameraController>();
	cameraObj->GetTransform()->Position = { 0, 0, 20 };
	cameraObj->GetTransform()->Rotation = { 0, radians(-90.0f), 0 }; // From euler angles
	TotalObjects++;

	// Default Cube Mesh //
	MeshRenderer::MeshInfo meshInfo;
	meshInfo.Mesh = Mesh::Cube();
	meshInfo.Material = {};
	meshInfo.Material.Albedo = { 1, 0, 0, 1 };

	// Create spring points
	PointA = new GameObject(CurrentScene(), "Spring Point A");
	Particle* particleA = PointA->AddComponent<Particle>();
	particleA->IsStatic = true;
	particleA->SetRestitution(0.0f);
	
	PointA->GetTransform()->Position = { 0, 2, 0 };
	PointA->GetTransform()->Scale = vec3(0.2f);
	PointA->AddComponent<MeshRenderer>()->Meshes = { meshInfo };

	PointB = new GameObject(CurrentScene(), "Spring Point B");
	Particle* particleB = PointB->AddComponent<Particle>();
	PointB->GetTransform()->Position = { 1, 0.5f, 0 };
	PointB->GetTransform()->Scale = vec3(0.2f);
	PointB->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	particleB->SetRestitution(0.0f);
	PointB->AddComponent<BoxCollider>();

	// Create spring
	DemoSpring = PointA->AddComponent<Spring>();
	DemoSpring->SetRestingLength(SpringRestingLength);
	DemoSpring->SetConstants(SpringStiffness, SpringDampingFactor);
	DemoSpring->SetBodies(particleA, particleB);
}

void SpringDemo::OnShutdown()
{
	delete gridMesh;
}

RaycastHit Hit = {};
const float CameraSpeed = 5.0f;
const float CameraRotationSpeed = 5.0f;
void SpringDemo::OnUpdate()
{
	if (Input::IsKeyPressed(GLFW_KEY_F11))
		ToggleFullscreen();

	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Exit();

	if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		CurrentScene()->GetPhysics().TogglePause();

	float SpringMoveSpeed = 2.5f * Renderer::GetDeltaTime();
	if (Input::IsKeyDown(GLFW_KEY_LEFT))  PointA->GetTransform()->Position.x -= SpringMoveSpeed;
	if (Input::IsKeyDown(GLFW_KEY_RIGHT)) PointA->GetTransform()->Position.x += SpringMoveSpeed;
	if (Input::IsKeyDown(GLFW_KEY_UP))    PointA->GetTransform()->Position.y += SpringMoveSpeed;
	if (Input::IsKeyDown(GLFW_KEY_DOWN))  PointA->GetTransform()->Position.y -= SpringMoveSpeed;

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

void SpringDemo::OnDraw()
{
	static bool controlsWindowOpen = true;
	if (ImGui::Begin("Controls", &controlsWindowOpen))
	{
		ImGui::Text("Space:       Toggle pause physics");
		ImGui::Text("Arrow Keys:  Move object A");
		ImGui::Text("WASD:        Move camera");
		ImGui::Text("Q/E:         Move camera up/down");
		ImGui::Text("Right Mouse: Hold and move mouse to look around");
		ImGui::Text("F:           Applies force at blue box position");
		ImGui::Text("F11:         Toggle fullscreen");
		ImGui::End();
	}

	static bool springWindowOpen = true;
	if (ImGui::Begin("Spring Demo", &springWindowOpen))
	{
		if (ImGui::SliderFloat("Stiffness", &SpringStiffness, 0.0f, 100.0f))
			DemoSpring->SetConstants(SpringStiffness, SpringDampingFactor);
		if (ImGui::SliderFloat("Dampening", &SpringDampingFactor, 0.0f, 1.0f))
			DemoSpring->SetConstants(SpringStiffness, SpringDampingFactor);
		if (ImGui::SliderFloat("Resting Length", &SpringRestingLength, 0.1f, 100.0f))
			DemoSpring->SetRestingLength(SpringRestingLength);

		ImGui::End();
	}
}

void SpringDemo::OnDrawGizmos()
{
	if (!gridMesh)
		gridMesh = Mesh::Grid(GridSize);

	Gizmos::Colour = { 1, 1, 1, 0.2f };
	Gizmos::Draw(gridMesh, vec3(-GridSize, -5.0f, -GridSize), vec3(GridSize * 2.0f));

	if (Hit.Distance > 0)
	{
		Gizmos::Colour = { 0, 1, 1, 1 };
		Gizmos::DrawCube(Hit.Point, vec3(0.025f));
	}
}