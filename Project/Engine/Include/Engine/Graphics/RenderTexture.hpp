#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Engine::Graphics
{
	enum class TextureFormat
	{
		None = 0,

		// Colour
		RGBA8,
		RGBA16,
		RGBA16F,
		RedInteger,

		Cubemap,

		// Depth & Stencil
		Depth24Stencil8,

		// Aliases
		Depth = Depth24Stencil8
	};

	bool IsDepthFormat(TextureFormat format);
	GLenum GetTextureTarget(TextureFormat format, bool multisampled = false);
	GLenum TextureFormatToGLFormat(TextureFormat format);
	GLenum TextureFormatToInternalGLFormat(TextureFormat format);

	class RenderTexture
	{
		glm::ivec2 m_Resolution;
		unsigned int m_ID, m_Samples = 1;
		TextureFormat m_Format = TextureFormat::RGBA8;

		void Recreate();

		void CreateDepthTexture();
		void CreateColourTexture();

	public:
		RenderTexture(
			glm::ivec2 resolution,
			TextureFormat format = TextureFormat::RGBA8,
			unsigned int samples = 1);
		~RenderTexture();

		unsigned int GetID();

		void Bind(unsigned int textureIndex = 0);
		void Unbind();

		void CopyTo(RenderTexture* destination);

		unsigned int GetSamples();
		void SetSamples(unsigned int samples);

		TextureFormat GetFormat();
		void SetFormat(TextureFormat format);

		glm::ivec2 GetResolution();
		void SetResolution(glm::ivec2 newResolution);
	};
}