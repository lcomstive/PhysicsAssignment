#include <Engine/Graphics/Mesh.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Graphics/Pipelines/Deferred.hpp>

#include <Engine/Components/Light.hpp>
#include <Engine/Components/Transform.hpp>

using namespace glm;
using namespace std;
using namespace Engine;
using namespace Engine::Graphics;
using namespace Engine::Components;
using namespace Engine::Graphics::Pipelines;

DeferredRenderPipeline::DeferredRenderPipeline()
{
	FramebufferSpec framebuffer = { Renderer::GetResolution()};

	// Mesh Pass //
	framebuffer.Attachments =
	{
		TextureFormat::RGBA16F,	// Position
		TextureFormat::RGBA16F,	// Normal
		TextureFormat::RGBA8,	// Albedo w/ roughness in alpha channel
		TextureFormat::RGBA8,	// Ambient Occlusion w/ metalness in alpha channel
		TextureFormat::Depth
	};

	m_MeshPass = new RenderPass(framebuffer);

	Shader* meshShader = new Shader(ShaderStageInfo
		{
			"./Assets/Shaders/Deferred/Mesh.vert",
			"./Assets/Shaders/Deferred/Mesh.frag"
		});

	RenderPipelinePass pass = { meshShader , m_MeshPass };
	pass.DrawCallback = std::bind(&DeferredRenderPipeline::MeshPass, this);

	AddPass(pass);

	// Lighting Pass //
	framebuffer.Attachments =
	{
		TextureFormat::RGBA8,
		TextureFormat::Depth
	};
	m_LightingPass = new RenderPass(framebuffer);
	pass.Pass = m_LightingPass;

	pass.DrawCallback = bind(&DeferredRenderPipeline::LightingPass, this);

	pass.Shader = new Shader(
		ShaderStageInfo
		{
			"./Assets/Shaders/Deferred/Lighting.vert",
			"./Assets/Shaders/Deferred/Lighting.frag",
		});
	AddPass(pass);
}

DeferredRenderPipeline::~DeferredRenderPipeline()
{
	RemovePass(m_MeshPass);
	RemovePass(m_ForwardPass);
	RemovePass(m_LightingPass);

	delete m_MeshPass;
	delete m_ForwardPass;
	delete m_LightingPass;
}

void DeferredRenderPipeline::MeshPass()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer::Draw();
}

const unsigned int MaxLights = 16;
void DeferredRenderPipeline::LightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* shader = CurrentShader();

	// FILL G-BUFFER MAPS //
	// Position
	m_MeshPass->GetColourAttachment()->Bind();
	shader->Set("inputPosition", 0);

	// Normals
	m_MeshPass->GetColourAttachment(1)->Bind(1);
	shader->Set("inputNormal", 1);

	// Albedo + Roughness
	m_MeshPass->GetColourAttachment(2)->Bind(2);
	shader->Set("inputAlbedoRoughness", 2);

	// Ambient Occlusion + Metalness
	m_MeshPass->GetColourAttachment(3)->Bind(3);
	shader->Set("inputAmbientMetalness", 3);

	// Depth
	m_MeshPass->GetDepthAttachment()->Bind(4);
	shader->Set("inputDepth", 4);

	// FILL LIGHT DATA //
	auto lights = Renderer::GetApp()->CurrentScene()->Root().GetComponentsInChildren<Light>();

	unsigned int lightIndex = 0;
	unsigned int lightCount = std::min((unsigned int)lights.size(), MaxLights);
	shader->Set("lightCount", (int)lightCount);
	for (Light* light : lights)
	{
		shader->Set("lights[" + to_string(lightIndex) + "].Colour", light->Colour);
		shader->Set("lights[" + to_string(lightIndex) + "].Radius", light->Radius);
		shader->Set("lights[" + to_string(lightIndex) + "].Position", light->GetTransform()->Position);

		if (++lightIndex >= MaxLights)
			break;
	}

	// DRAW FULLSCREEN QUAD //
	Mesh::Quad()->Draw();
}