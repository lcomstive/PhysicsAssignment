#version 330 core
out vec4 fragColour;

in vec2 passTexCoords;

uniform sampler2D inputTexture;

void main()
{
	fragColour = texture(inputTexture, passTexCoords);
}