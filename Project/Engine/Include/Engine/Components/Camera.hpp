#pragma once
#include <glm/glm.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Components/Component.hpp>
#include <Engine/Graphics/RenderTexture.hpp>

namespace Engine::Components
{
	struct Camera : public Component
	{
		Camera();

		float FieldOfView = 60.0f;

		float ClipNear = 0.1f;
		float ClipFar = 1000.0f;

		bool Orthographic = false;
		float OrthoSize = 5.0f;

		Graphics::RenderTexture* RenderTarget = nullptr;

		glm::mat4 GetViewMatrix();
		void FillShader(Graphics::Shader* shader);

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();
		glm::mat4 GetProjectionMatrix();
		
		void SetMainCamera();
		static Camera* GetMainCamera();

	private:
		static Camera* s_MainCamera;
	};
}