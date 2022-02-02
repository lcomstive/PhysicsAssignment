#include <Engine/Log.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Graphics/Texture.hpp>
#include <Engine/Graphics/Material.hpp>

using namespace std;
using namespace Engine;
using namespace Engine::Graphics;

void BindTexture(unsigned int index, string shaderName, Texture* texture, Shader* shader)
{
	shader->Set(shaderName, (int)index);
	glActiveTexture(GL_TEXTURE0 + index);
	
	if (texture)
		texture->Bind(index);
	else
		glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::FillShader(Shader* shader)
{
	if (!shader)
		return;

	shader->Set("albedoColour", Albedo);

	/*
	shader->Set("alphaClipping", AlphaClipping);
	shader->Set("alphaClipThreshold", AlphaClipThreshold);
	shader->Set("textureCoordScale", TextureCoordinateScale);
	shader->Set("textureCoordOffset", TextureCoordinateOffset);
	
	shader->Set("hasAlbedoMap", AlbedoMap != nullptr);
	shader->Set("hasNormalMap", NormalMap != nullptr);

	BindTexture(0, "albedoMap", AlbedoMap, shader);
	BindTexture(1, "normalMap", NormalMap, shader);
	*/
}
