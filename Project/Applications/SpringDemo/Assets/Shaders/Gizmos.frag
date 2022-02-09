#version 330 core

out vec4 FragColour;

uniform vec4 albedoColour;

void main()
{
	FragColour = vec4(albedoColour);
}
