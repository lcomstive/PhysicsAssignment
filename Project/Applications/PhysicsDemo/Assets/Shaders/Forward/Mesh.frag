#version 330 core
#include "./Assets/Shaders/Include/Material.inc"

out vec4 FragColour;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normals;

void main()
{
	vec3 Albedo = texture(albedoMap, TexCoords).rgb;
	vec3 WorldPos = normalize(WorldPos);
	vec3 Normals = normalize(Normals);

	FragColour = albedoColour;
	
	// Check for alpha clipping
	if(alphaClipping && FragColour.a <= alphaClipThreshold)
		discard;
}