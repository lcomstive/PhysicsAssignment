#version 330 core
#include "ASSET_DIR/Shaders/Include/Camera.inc"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * modelMatrix * vec4(position, 1.0);
}