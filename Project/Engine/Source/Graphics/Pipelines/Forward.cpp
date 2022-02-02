#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Graphics/Pipelines/Forward.hpp>

using namespace glm;
using namespace std;
using namespace Engine;
using namespace Engine::Graphics;
using namespace Engine::Components;
using namespace Engine::Graphics::Pipelines;

ForwardRenderPipeline::ForwardRenderPipeline() 
{
	FramebufferSpec framebuffer = { Renderer::GetResolution() };
	framebuffer.Attachments =
	{
		TextureFormat::RGBA8,
		TextureFormat::Depth
	};
	framebuffer.SwapchainTarget = true;
	
	Shader* shader = new Shader(ShaderStageInfo
		{
			"./Assets/Shaders/Forward/Mesh.vert",
			"./Assets/Shaders/Forward/Mesh.frag"
		});
	RenderPipelinePass pass = { shader, new RenderPass(framebuffer) };
	pass.DrawCallback = bind(&ForwardRenderPipeline::ForwardPass, this);

	AddPass(pass);
}

ForwardRenderPipeline::~ForwardRenderPipeline()
{
	RenderPass* forwardPass = m_RenderPasses[0].Pass;
	RemovePass(forwardPass);
	delete forwardPass;
}

void ForwardRenderPipeline::ForwardPass()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer::Draw();
}