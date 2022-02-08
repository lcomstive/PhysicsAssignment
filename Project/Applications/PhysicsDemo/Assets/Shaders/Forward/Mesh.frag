#version 330 core
#include "./Assets/Shaders/Include/Material.inc"

out vec4 FragColour;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normals;

uniform float time;

float remap(float value, float low1, float high1, float low2, float high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

void main()
{
	vec3 Albedo = texture(albedoMap, TexCoords).rgb;
	vec3 WorldPos = normalize(WorldPos);
	vec3 Normals = normalize(Normals);

	FragColour = albedoColour;

	FragColour = vec4(vec3(remap(sin(time), -1, 1, -1, -0.5)) * WorldPos, 1);
	
	// Check for alpha clipping
	if(alphaClipping && FragColour.a <= alphaClipThreshold)
		discard;
}