#include <imgui.h>
#include "PhysicsDemo.hpp"
#include <Engine/Utilities.hpp>
#include <Engine/Physics/Shapes.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Components/Physics/Particle.hpp>
#include <Engine/Components/Physics/Rigidbody.hpp>
#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/Graphics/MeshRenderer.hpp>
#include <Engine/Components/Physics/PlaneCollider.hpp>
#include <Engine/Components/OrbitCameraController.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>
#include <Engine/Physics/Broadphase/BroadphaseOctree.hpp>

#pragma warning(disable : 4244)

#define TOWER_DEMO			1
#define DISTANCE_JOINT_DEMO 0

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

PhysicsDemo::PhysicsDemo(ApplicationArgs args) : Application(args) { }

// Walls
const float WallDistance = 100.0f, WallThickness = 5;
const vec3 WallOffset = { 0, WallDistance - WallThickness - 10.0f, 0 };
Material WallMaterial = {};

// Grid
Mesh* gridMesh = nullptr;
const int GridSize = 250;

#if DISTANCE_JOINT_DEMO
#include <Engine/Components/Physics/Constraints/DistanceJoint.hpp>

float DistanceJointLength = 3.0f;
DistanceJoint* DemoDistanceJoint = nullptr;
#endif

#if DISTANCE_JOINT_DEMO
GameObject* PointA = nullptr;
GameObject* PointB = nullptr;
#endif

unsigned int TotalObjects = 0;
void PhysicsDemo::OnStart()
{
	// Set ImGui Font
	ImGui::GetIO().Fonts->AddFontFromFileTTF("./Assets/Fonts/Source Sans Pro/SourceSansPro-Regular.ttf", 16.0f);

	// Default Cube Mesh //
	MeshRenderer::MeshInfo meshInfo;
	meshInfo.Mesh = Mesh::Cube();
	meshInfo.Material = {};

	CreateWalls();

	// Main Camera //
	GameObject* cameraObj = new GameObject(CurrentScene(), "Main Camera");
	cameraObj->AddComponent<OrbitCameraController>();
	cameraObj->GetTransform()->Position = { 0, 0, 20 };
	cameraObj->GetTransform()->Rotation = { 0, radians(-90.0f), 0 }; // From euler angles
	TotalObjects++;

	meshInfo.Material.Albedo = { 1, 0, 0, 1 };

#if TOWER_DEMO
#ifdef NDEBUG
	const int TowerSize = 10;
#else
	const int TowerSize = 5;
#endif
	for (int x = 0; x < TowerSize; x++)
	{
		for (int y = 0; y < TowerSize; y++)
		{
			for (int z = 0; z < TowerSize; z++)
			{
				GameObject* go = new GameObject(CurrentScene(), "Tower " + to_string(y));
				go->GetTransform()->Position.x = (x - TowerSize / 2.0f) * 5;
				go->GetTransform()->Position.y = y * 5;
				go->GetTransform()->Position.z = (z - TowerSize / 2.0f) * 5;
				go->GetTransform()->Rotation = radians(vec3{ Random(-180, 180), Random(-180, 180), Random(-180, 180) });

				Rigidbody* rb = go->AddComponent<Rigidbody>();
				rb->SetRestitution(0.9f);

				bool sphere = rand() % 2 == 0;
				if (sphere)
				{
					meshInfo.Mesh = Mesh::Sphere();
					go->AddComponent<SphereCollider>();
				}
				else
				{
					meshInfo.Mesh = Mesh::Cube();
					BoxCollider* bCollider = go->AddComponent<BoxCollider>();					
				}

				go->AddComponent<MeshRenderer>()->Meshes = { meshInfo };

				TotalObjects++;
			}
		}
	}
#endif

#if DISTANCE_JOINT_DEMO
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

	DemoDistanceJoint = PointA->AddComponent<DistanceJoint>();
	DemoDistanceJoint->Initialise(particleA, particleB, DistanceJointLength);
#endif
}

void PhysicsDemo::OnShutdown()
{
	delete gridMesh;
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
	speed *= Renderer::GetDeltaTime();

	if (Input::IsKeyDown(GLFW_KEY_A)) transform->Position.x -= speed;
	if (Input::IsKeyDown(GLFW_KEY_D)) transform->Position.x += speed;
	if (Input::IsKeyDown(GLFW_KEY_W)) transform->Position.z -= speed;
	if (Input::IsKeyDown(GLFW_KEY_S)) transform->Position.z += speed;

	if (Input::IsKeyDown(GLFW_KEY_Q)) transform->Position.y -= speed;
	if (Input::IsKeyDown(GLFW_KEY_E)) transform->Position.y += speed;

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

#if DISTANCE_JOINT_DEMO
	float SpringMoveSpeed = 2.5f * Renderer::GetDeltaTime();
	if (Input::IsKeyDown(GLFW_KEY_LEFT))  PointA->GetTransform()->Position.x -= SpringMoveSpeed;
	if (Input::IsKeyDown(GLFW_KEY_RIGHT)) PointA->GetTransform()->Position.x += SpringMoveSpeed;
	if (Input::IsKeyDown(GLFW_KEY_UP))    PointA->GetTransform()->Position.y += SpringMoveSpeed;
	if (Input::IsKeyDown(GLFW_KEY_DOWN))  PointA->GetTransform()->Position.y -= SpringMoveSpeed;
#endif

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
	static bool controlsWindowOpen = true;
	if (ImGui::Begin("Controls", &controlsWindowOpen))
	{
		ImGui::Text("Space:		  Toggle pause physics");
		ImGui::Text("WASD:		  Move camera");
		ImGui::Text("Q/E:		  Move camera up/down");
		ImGui::Text("Right Mouse: Hold and move mouse to look around");
		ImGui::Text("F:			  Applies force at blue box position");
		ImGui::Text("F11:		  Toggle fullscreen");
		ImGui::End();
	}

// #ifndef NDEBUG
	const ImVec4 ColourGood = { 1, 1, 1, 1 };
	const ImVec4 ColourBad = { 1, 0, 0, 1 };

	static bool debugWindowOpen = true;
	PhysicsSystem& physicsSystem = CurrentScene()->GetPhysics();
	if (ImGui::Begin("Debugging", &debugWindowOpen))
	{
		float frameTime = Renderer::GetDeltaTime() * 1000.0f;
		float lastTimeStep = physicsSystem.LastTimeStep().count();
		float desiredTimestep = physicsSystem.Timestep().count();

		ImGui::Text("FPS: %f\n", Renderer::GetFPS());
		ImGui::Text("Total Objects: %i", TotalObjects);
		ImGui::TextColored(
			(frameTime < (1000.0f / 30.0f)) ? ColourGood : ColourBad,
			"Render  Frame Time: %.1fms",
			frameTime);
		ImGui::TextColored(
			(lastTimeStep <= desiredTimestep) ? ColourGood : ColourBad,
			"Physics Frame Time: %.1fms / %.1fms",
			lastTimeStep, desiredTimestep
		);
		ImGui::Text("Resolution: (%d, %d)", Renderer::GetResolution().x, Renderer::GetResolution().y);
		ImGui::Text("VSync: %s", Renderer::GetVSync() ? "Enabled" : "Disabled");
		ImGui::Text("Samples: %d", Renderer::GetSamples());

		if (CurrentScene()->GetPhysics().GetState() == PhysicsPlayState::Paused)
			ImGui::Text("PHYSICS PAUSED");

		ImGui::End();
	}
// #endif

#if DISTANCE_JOINT_DEMO
	static bool distanceJointWindowOpen = true;
	if (ImGui::Begin("Spring Demo", &distanceJointWindowOpen))
	{
		if (ImGui::SliderFloat("Distance", &DistanceJointLength, 0.0f, 100.0f))
			DemoDistanceJoint->SetLength(DistanceJointLength);

		ImGui::End();
	}
#endif
}

void PhysicsDemo::OnDrawGizmos()
{
	/*
	if (!gridMesh)
		gridMesh = Mesh::Grid(GridSize);

	Gizmos::Colour = { 1, 1, 1, 0.2f };
	Gizmos::Draw(gridMesh, vec3(-GridSize, -WallOffset.y / 2.0f, -GridSize), vec3(GridSize * 2.0f));
	*/

	if (Hit.Distance > 0)
	{
		Gizmos::Colour = { 0, 1, 1, 1 };
		Gizmos::DrawCube(Hit.Point, vec3(0.025f));
	}
}

void PhysicsDemo::CreateWall(vec3 axis)
{
	WallMaterial.Albedo.a = 0.1f;

	GameObject* wall = new GameObject(CurrentScene(), "Wall");
	// wall->AddComponent<MeshRenderer>()->Meshes = { MeshRenderer::MeshInfo { Mesh::Cube(), WallMaterial }};
	wall->AddComponent<BoxCollider>();

	Transform* transform = wall->GetTransform();
	transform->Scale = vec3(WallDistance) - abs(axis) * vec3(WallDistance - WallThickness);
	transform->Rotation = axis * radians(90.0f); // Rotate 90 degrees on axis
	transform->Position = -axis * WallDistance + WallOffset;

	TotalObjects++;
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
