#version 330 core
#include "ASSET_DIR/Shaders/Include/Material.inc"

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

void main()
{
	gPosition = WorldPos;
	gNormal = Normal;

	if(hasAlbedoMap)
		gAlbedo = texture(albedoMap, TexCoords).rgb;
	else
		gAlbedo = albedoColour.rgb;
}
