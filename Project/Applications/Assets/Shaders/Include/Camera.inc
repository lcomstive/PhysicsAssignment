struct Camera
{
	vec3 Position;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

uniform Camera camera;
uniform float gamma = 2.2;

// Low to high dynamic range
vec3 LDRToHDR(vec3 colour)
{
	colour = colour / (colour + vec3(1.0));
	colour = pow(colour, vec3(1.0 / gamma));
	return colour;
}
