#version 330 core

out vec4 fragColour;

in vec2 passTexCoords;

uniform sampler2D inputPosition;
uniform sampler2D inputNormal;
uniform sampler2D inputAlbedo;
uniform sampler2D inputDepth;

void main()
{
	vec3 Albedo = texture(inputAlbedo, passTexCoords).rgb;
	vec3 WorldPos = texture(inputPosition, passTexCoords).rgb;
	vec3 Normals = texture(inputNormal, passTexCoords).rgb;

	fragColour = vec4(Albedo, 1.0);
}