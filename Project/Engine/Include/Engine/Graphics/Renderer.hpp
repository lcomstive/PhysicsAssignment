#pragma once
#include <mutex>
#include <vector>
#include <glm/glm.hpp>
#include <Engine/Graphics/Mesh.hpp>
#include <Engine/Graphics/Material.hpp>
#include <Engine/Components/Camera.hpp>
#include <Engine/Components/Transform.hpp>
#include <Engine/Graphics/RenderPipeline.hpp>

namespace Engine::Graphics
{
	struct DrawCall
	{
		Mesh* Mesh;
		Material Material;

		glm::vec3 Position, Scale, Rotation;
	};

	class Renderer
	{
		static uint32_t s_Samples;
		static Application* s_App;
		static glm::ivec2 s_Resolution;
		static bool s_Wireframe, s_VSync;
		static RenderPipeline* s_Pipeline;
		static std::vector<DrawCall> s_DrawQueue;
		static double s_Time, s_FPS, s_DeltaTime;

		static void Shutdown();
		static void Resized(glm::ivec2 newResolution);

		friend class Engine::Application;

	public:
		static void Draw(bool clearQueue = true);

		static void ClearDrawQueue();
		static void Submit(DrawCall drawCall);
		static void Submit(Mesh* mesh, Material& material, Components::Transform* transform);

#pragma region Setters
		static void SetVSync(bool vsync = true);
		static void SetWireframe(bool wireframe = true);

		template<typename T>
		static T* SetPipeline()
		{
			Log::Assert(std::is_base_of<RenderPipeline, T>(), "Pipeline needs to inherit from Engine::Graphics::RenderPipeline");
			if (s_Pipeline)
				delete s_Pipeline;
			s_Pipeline = new T();
			return (T*)s_Pipeline;
		}
#pragma endregion

#pragma region Getters
		static bool GetVSync();
		static double GetFPS();
		static double GetTime();
		static uint32_t GetSamples();
		static double GetDeltaTime();
		static Application* GetApp();
		static bool GetWireframeMode();
		static glm::ivec2 GetResolution();
		static RenderPipeline* GetPipeline();
#pragma endregion
	};
}
