#include <Engine/Application.hpp>
#include <Engine/Graphics/RenderPipeline.hpp>

namespace Engine::Graphics::Pipelines
{
	class ForwardRenderPipeline : public RenderPipeline
	{
		void ForwardPass();
		
	public:
		ForwardRenderPipeline();
		~ForwardRenderPipeline();
	};
}
