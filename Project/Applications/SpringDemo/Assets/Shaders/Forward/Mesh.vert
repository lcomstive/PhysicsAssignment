#version 330 core
#include "./Assets/Shaders/Include/Camera.inc"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normals;
out vec3 WorldPos;

uniform mat4 modelMatrix;

uniform vec2 textureCoordScale;
uniform vec2 textureCoordOffset;

void main()
{
	Normals = normals;
	WorldPos = vec3(modelMatrix * vec4(position, 1.0));
	TexCoords = (texCoords * textureCoordScale) + textureCoordOffset;

	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * modelMatrix * vec4(position, 1.0);
}