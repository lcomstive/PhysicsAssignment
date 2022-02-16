#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Engine/Graphics/Renderer.hpp>

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Graphics;

float Renderer::s_FPS = 0;
Application* Renderer::s_App = nullptr;
float Renderer::s_Time = 0;
bool Renderer::s_VSync = false;
uint32_t Renderer::s_Samples = 1;
float Renderer::s_DeltaTime = 0;
bool Renderer::s_Wireframe = false;
ivec2 Renderer::s_Resolution = { 0, 0 };
vector<DrawCall> Renderer::s_DrawQueue = {};
RenderPipeline* Renderer::s_Pipeline = nullptr;

void Renderer::SetVSync(bool vsync) { glfwSwapInterval(s_VSync = vsync ? 1 : 0); }
void Renderer::SetWireframe(bool wireframe) { glPolygonMode(GL_FRONT_AND_BACK, (s_Wireframe = wireframe) ? GL_LINE : GL_FILL); }

float Renderer::GetFPS() { return s_FPS; }
float Renderer::GetTime() { return s_Time; }
bool Renderer::GetVSync() { return s_VSync; }
uint32_t Renderer::GetSamples() { return s_Samples; }
float Renderer::GetDeltaTime() { return s_DeltaTime; }
Application* Renderer::GetApp() { return s_App; }
ivec2 Renderer::GetResolution() { return s_Resolution; }
bool Renderer::GetWireframeMode() { return s_Wireframe; }
RenderPipeline* Renderer::GetPipeline() { return s_Pipeline; }

void Renderer::Shutdown()
{
	if (s_Pipeline)
		delete s_Pipeline;
}

void Renderer::Draw(bool clearQueue)
{
	Shader* shader = s_Pipeline->CurrentShader();
	for (unsigned int i = 0; i < (unsigned int)s_DrawQueue.size(); i++)
	{
		// Generate model matrix
		mat4 translationMatrix = translate(mat4(1.0f), s_DrawQueue[i].Position);
		mat4 scaleMatrix = scale(mat4(1.0f), s_DrawQueue[i].Scale);

		shader->Set("modelMatrix", translationMatrix * s_DrawQueue[i].Rotation * scaleMatrix);

		// Fill material values
		s_DrawQueue[i].Material.FillShader(shader);

		Renderer::SetWireframe(s_DrawQueue[i].Material.Wireframe);

		s_DrawQueue[i].Mesh->Draw();

		if (s_DrawQueue[i].DeleteMeshAfterRender)
			delete s_DrawQueue[i].Mesh;
	}

	if (clearQueue)
		ClearDrawQueue();
}

void Renderer::Resized(glm::ivec2 newResolution)
{
	s_Resolution = newResolution;
	if (s_Pipeline)
		s_Pipeline->OnResized(s_Resolution);
}

void Renderer::ClearDrawQueue()
{
	s_DrawQueue.clear();
}

void Renderer::Submit(Mesh* mesh, Material& material, vec3 position, vec3 scale, vec3 rotation)
{
	Submit(mesh, material, position, scale, eulerAngleXYZ(rotation.x, rotation.y, rotation.z));
}

void Renderer::Submit(Mesh* mesh, Material& material, vec3 position, vec3 scale, mat4 rotation)
{
	Submit(DrawCall
		{
			mesh,
			material,
			position,
			scale,
			rotation
		});
}


void Renderer::Submit(Mesh* mesh, Material& material, Components::Transform* transform)
{
	Submit(DrawCall
		{
			mesh,
			material,
			transform->GetGlobalPosition(),
			transform->GetGlobalScale(),
			transform->GetGlobalRotationMatrix()
		});
}

void Renderer::Submit(DrawCall drawCall) { s_DrawQueue.emplace_back(drawCall); }