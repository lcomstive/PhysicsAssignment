#version 330 core

out vec4 fragColour;

in vec2 passTexCoords;

uniform sampler2D inputPosition;
uniform sampler2D inputNormal;
uniform sampler2D inputAlbedoRoughness;
uniform sampler2D inputAmbientMetalness;
uniform sampler2D inputDepth;

void main()
{
	vec3 Albedo = texture(inputAlbedoRoughness, passTexCoords).rgb;
	vec3 WorldPos = texture(inputPosition, passTexCoords).rgb;
	vec3 Normals = texture(inputNormal, passTexCoords).rgb;
	vec3 Roughness = texture(inputAlbedoRoughness, passTexCoords).a;
	vec3 Metalness = texture(inputAmbientMetalness, passTexCoords).a;
	vec3 AmbientOcclusion = texture(inputAmbientMetalness, passTexCoords).rgb;

	fragColour = vec4(Albedo, 1.0);
}