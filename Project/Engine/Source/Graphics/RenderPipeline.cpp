#include <Engine/Graphics/Mesh.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Components/Camera.hpp>
#include <Engine/Graphics/RenderPipeline.hpp>

using namespace glm;
using namespace std;
using namespace Engine;
using namespace Engine::Graphics;
using namespace Engine::Components;

void RenderPipeline::Draw(Camera& camera)
{
	m_PreviousPass = nullptr;
	for(unsigned int i = 0; i < (unsigned int)m_RenderPasses.size(); i++)
	{
		RenderPipelinePass& info = m_RenderPasses[i];

		// Setup
		info.Pass->Begin();

		m_CurrentShader = info.Shader;
		if (m_CurrentShader)
		{
			m_CurrentShader->Bind();

			m_CurrentShader->Set("resolution", vec2 { Renderer::GetResolution().x, Renderer::GetResolution().y });
			m_CurrentShader->Set("time", Renderer::GetTime());
			m_CurrentShader->Set("deltaTime", Renderer::GetDeltaTime());

			camera.FillShader(m_CurrentShader);
		}

		// Draw calls
		if (info.DrawCallback)
			info.DrawCallback();

		// Finalise
		if (m_CurrentShader)
			m_CurrentShader->Unbind();

		m_CurrentShader = nullptr;
		info.Pass->End();
		m_PreviousPass = info.Pass;
	}
	m_PreviousPass = nullptr;

	if (camera.RenderTarget && !m_RenderPasses.empty())
		m_RenderPasses[m_RenderPasses.size() - 1].Pass->GetFramebuffer()->CopyAttachmentTo(camera.RenderTarget);
	else if(!m_RenderPasses.empty())
		m_RenderPasses[m_RenderPasses.size() - 1].Pass->GetFramebuffer()->BlitTo(nullptr, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderPipeline::RemovePass(RenderPass* pass)
{
	for (int i = (int)m_RenderPasses.size() - 1; i >= 0; i--)
	{
		if (m_RenderPasses[i].Pass == pass)
		{
			m_RenderPasses.erase(m_RenderPasses.begin() + i);
			break;
		}
	}
}

void RenderPipeline::AddPass(RenderPipelinePass& passInfo)
{
	if (passInfo.Pass)
		m_RenderPasses.emplace_back(passInfo);
}

RenderTexture* RenderPipeline::GetOutputAttachment(unsigned int index)
{
	if (m_RenderPasses.empty())
		return nullptr;

	RenderPass* pass = m_RenderPasses[m_RenderPasses.size() - 1].Pass;
	index = std::clamp(index, 0u, pass->GetColourAttachmentCount());
	return pass->GetColourAttachment(index);
}

void RenderPipeline::OnResized(ivec2 resolution)
{
	for (RenderPipelinePass& pass : m_RenderPasses)
		if (pass.ResizeWithScreen)
			pass.Pass->GetFramebuffer()->SetResolution(resolution);
}

Shader* RenderPipeline::CurrentShader() { return m_CurrentShader; }
RenderPass* RenderPipeline::GetPreviousPass() { return m_PreviousPass; }