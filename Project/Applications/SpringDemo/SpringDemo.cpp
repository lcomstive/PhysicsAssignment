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

Spring* DemoSpring1 = nullptr;
Spring* DemoSpring2 = nullptr;

float SpringStiffness[2] = { 10.0f, 10.0f };
float SpringRestingLength[2] = { 1.0f, 1.0f };
float SpringDampingFactor[2] = { 0.5f, 0.5f };

GameObject* PointA = nullptr;
GameObject* PointB = nullptr;
GameObject* PointC = nullptr;

unsigned int TotalObjects = 0;
void SpringDemo::OnStart()
{
	// Set ImGui Font
	ImGui::GetIO().Fonts->AddFontFromFileTTF((AssetDir + "Fonts/Source Sans Pro/SourceSansPro-Regular.ttf").c_str(), 16.0f);
	
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
	SPRING_PARTICLE_TYPE* particleA = PointA->AddComponent<SPRING_PARTICLE_TYPE>();
	particleA->SetStatic(true);
	
	PointA->GetTransform()->Position = { 0, 2, 0 };
	PointA->GetTransform()->Scale = vec3(0.2f);
	PointA->AddComponent<MeshRenderer>()->Meshes = { meshInfo };

	PointB = new GameObject(CurrentScene(), "Spring Point B");
	SPRING_PARTICLE_TYPE* particleB = PointB->AddComponent<SPRING_PARTICLE_TYPE>();
	PointB->GetTransform()->Position = { 1, 0.5f, 0 };
	PointB->GetTransform()->Scale = vec3(0.1f);
	PointB->AddComponent<MeshRenderer>()->Meshes = { meshInfo };

	PointC = new GameObject(CurrentScene(), "Spring Point C");
	SPRING_PARTICLE_TYPE* particleC = PointC->AddComponent<SPRING_PARTICLE_TYPE>();
	PointC->GetTransform()->Position = { -1, 0.5f, 0 };
	PointC->GetTransform()->Scale = vec3(0.1f);
	PointC->AddComponent<MeshRenderer>()->Meshes = { meshInfo };

	// Create springs
	DemoSpring1 = PointA->AddComponent<Spring>();
	DemoSpring1->SetRestingLength(SpringRestingLength[0]);
	DemoSpring1->SetConstants(SpringStiffness[0], SpringDampingFactor[0]);
	DemoSpring1->SetBodies(particleA, particleB);
	
	DemoSpring2 = PointC->AddComponent<Spring>();
	DemoSpring2->SetRestingLength(SpringRestingLength[1]);
	DemoSpring2->SetConstants(SpringStiffness[1], SpringDampingFactor[1]);
	DemoSpring2->SetBodies(particleA, particleC);
}

void SpringDemo::OnShutdown() { delete gridMesh; }

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
		
		if (CurrentScene()->GetPhysics().GetState() == PhysicsPlayState::Paused)
			ImGui::Text("PHYSICS PAUSED");
		
		ImGui::End();
	}

	static bool springWindowOpen = true;
	if (ImGui::Begin("Spring Demo", &springWindowOpen))
	{
		if (ImGui::SliderFloat("Stiffness 1", &SpringStiffness[0], 0.0f, 1000.0f))
			DemoSpring1->SetConstants(SpringStiffness[0], SpringDampingFactor[0]);
		if (ImGui::SliderFloat("Dampening 1", &SpringDampingFactor[0], 0.0f, 1.0f))
			DemoSpring1->SetConstants(SpringStiffness[0], SpringDampingFactor[0]);
		if (ImGui::SliderFloat("Resting Length 1", &SpringRestingLength[0], 0.1f, 10.0f))
			DemoSpring1->SetRestingLength(SpringRestingLength[0]);
		
		if (ImGui::SliderFloat("Stiffness 2", &SpringStiffness[1], 0.0f, 1000.0f))
			DemoSpring2->SetConstants(SpringStiffness[1], SpringDampingFactor[1]);
		if (ImGui::SliderFloat("Dampening 2", &SpringDampingFactor[1], 0.0f, 1.0f))
			DemoSpring2->SetConstants(SpringStiffness[1], SpringDampingFactor[1]);
		if (ImGui::SliderFloat("Resting Length 2", &SpringRestingLength[1], 0.1f, 10.0f))
			DemoSpring2->SetRestingLength(SpringRestingLength[1]);

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