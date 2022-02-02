#include <Engine/Application.hpp>
#include <Engine/Graphics/RenderPipeline.hpp>

namespace Engine::Graphics::Pipelines
{
	class DeferredRenderPipeline : public RenderPipeline
	{
		RenderPass *m_MeshPass, *m_LightingPass, *m_ForwardPass;

		void MeshPass();
		void LightingPass();
		void ForwardPass();

	public:
		DeferredRenderPipeline();
		~DeferredRenderPipeline();
	};
}