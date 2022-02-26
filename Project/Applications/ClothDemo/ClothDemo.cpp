#include <imgui.h>
#include "ClothDemo.hpp"
#include <Engine/Utilities.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Components/Physics/BoxCollider.hpp>
#include <Engine/Components/Graphics/MeshRenderer.hpp>
#include <Engine/Components/OrbitCameraController.hpp>
#include <Engine/Components/Physics/SphereCollider.hpp>
#include <Engine/Components/Physics/Constraints/Cloth.hpp>

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

float ClothStiffness = 100.0f;
float ClothSpacing = 0.5f;
vec3 ClothInitialPos = { 0, 7.5f, 0 };

#ifndef NDEBUG
int ClothSize = 10;
#else
int ClothSize = 20;
#endif

Cloth* m_Cloth = nullptr;

ClothDemo::ClothDemo(ApplicationArgs args) : Application(args) { }

void ClothDemo::OnStart()
{
	srand(time(0));
	// Set ImGui Font
	ImGui::GetIO().Fonts->AddFontFromFileTTF((AssetDir + "Fonts/Source Sans Pro/SourceSansPro-Regular.ttf").c_str(), 16.0f);

	// Main Camera //
	GameObject* cameraObj = new GameObject(CurrentScene(), "Main Camera");
	cameraObj->AddComponent<OrbitCameraController>();
	cameraObj->GetTransform()->Position = { 0, 0, 20 };
	cameraObj->GetTransform()->Rotation = { 0, radians(-90.0f), 0 }; // From euler angles

	GameObject* clothObj = new GameObject(CurrentScene(), "Cloth");
	clothObj->GetTransform()->Position = ClothInitialPos;

	m_Cloth = clothObj->AddComponent<Cloth>();
	m_Cloth->Initialize(ClothSize, ClothSpacing);

	m_Cloth->SetStructuralSprings(ClothStiffness, 0.0f);
	m_Cloth->SetShearSprings(ClothStiffness, 0.0f);
	m_Cloth->SetBendSprings(ClothStiffness, 0.0f);

	// Create floor
	MeshRenderer::MeshInfo meshInfo = { Mesh::Cube(), Material {} };
	GameObject* floor = new GameObject(CurrentScene(), "Floor");
	meshInfo.Material.Albedo = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	floor->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	floor->AddComponent<BoxCollider>();
	floor->GetTransform()->Position.y = -6;
	floor->GetTransform()->Scale = { 25, 1, 25 };

	// Generate random obstacles
	const unsigned int ObstacleCount = 10;
	const vec3 ObstacleSpawnExtents = vec3(3, 5, 3);
	for (int i = 0; i < ObstacleCount; i++)
	{
		GameObject* obstacle = new GameObject(CurrentScene(), "Obstacle " + to_string(i));
		meshInfo.Material.Albedo = { Random(0.5f, 1.0f), Random(0.5f, 1.0f), Random(0.5f, 1.0f), 1.0f };
		obstacle->GetTransform()->Position =
		{
			Random(-ObstacleSpawnExtents.x, ObstacleSpawnExtents.x),
			Random(-ObstacleSpawnExtents.y, ObstacleSpawnExtents.y),
			Random(-ObstacleSpawnExtents.z, ObstacleSpawnExtents.z)
		};
		const float Scale = 1.0f;
		obstacle->GetTransform()->Scale = vec3(Scale);

		if (Random(0.0f, 1.0f) > 0.5f)
		{
			obstacle->AddComponent<BoxCollider>();
			obstacle->GetTransform()->Rotation =
			{
				Random(0.0f, radians(360.0f)),
				Random(0.0f, radians(360.0f)),
				Random(0.0f, radians(360.0f))
			};
			meshInfo.Mesh = Mesh::Cube();
		}
		else
		{
			obstacle->AddComponent<SphereCollider>()->SetRadius(Scale);
			meshInfo.Mesh = Mesh::Sphere();
		}
		obstacle->AddComponent<MeshRenderer>()->Meshes = { meshInfo };
	}
}

void ClothDemo::OnShutdown() { }

RaycastHit Hit = {};
const float CameraSpeed = 5.0f;
const float CameraRotationSpeed = 5.0f;
void ClothDemo::OnUpdate()
{
	if (Input::IsKeyPressed(GLFW_KEY_F11))
		ToggleFullscreen();

	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Exit();

	if (Input::IsKeyPressed(GLFW_KEY_SPACE))
		CurrentScene()->GetPhysics().TogglePause();
}

void ClothDemo::OnDraw()
{
	const ImVec4 ColourGood = { 1, 1, 1, 1 };
	const ImVec4 ColourBad = { 1, 0, 0, 1 };

	static bool debugWindowOpen = true;
	PhysicsSystem& physicsSystem = CurrentScene()->GetPhysics();
	if (ImGui::Begin("Debugging", &debugWindowOpen))
	{
		float frameTime = Renderer::GetDeltaTime() * 1000.0f;
		float lastTimeStep = physicsSystem.LastTimestep().count();
		float desiredTimestep = physicsSystem.Timestep().count();

		ImGui::Text("FPS: %f\n", Renderer::GetFPS());
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

	}
	ImGui::End();

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
	}
	ImGui::End();

	static bool clothWindowOpen = true;
	if (ImGui::Begin("Cloth Demo", &clothWindowOpen))
	{
		bool clothDirty = false;

		if (ImGui::SliderFloat("Cloth Stiffness", &ClothStiffness, 0.0f, 1000.0f))
			m_Cloth->SetBendSprings(ClothStiffness, 0.0f);
	}
	ImGui::End();
}

void ClothDemo::OnDrawGizmos()
{

}