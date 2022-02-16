#version 330 core
#include "ASSET_DIR/Shaders/Include/Material.inc"

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoRoughness;
layout (location = 3) out vec4 gAmbientMetalness;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

void main()
{
	gPosition = WorldPos;
	gNormal = Normal;
	gAlbedoRoughness.rgb = texture(albedoMap, TexCoords).rgb;
	gAlbedoRoughness.a = roughness;

	gAmbientMetalness.rgb = vec3(0.0);
	gAmbientMetalness.a = metalness;

	if(hasRoughnessMap)
		gAlbedoRoughness.a = texture(roughnessMap, TexCoords).g;
	if(hasMetalnessMap)
		gAmbientMetalness.a = texture(metalnessMap, TexCoords).b;
	if(hasAmbientOcclusionMap)
		gAmbientMetalness.rgb = texture(ambientOcclusionMap, TexCoords).rgb;
}
