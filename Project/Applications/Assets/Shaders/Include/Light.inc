#include "/Assets/Shaders/Camera.inc"

const int MAX_LIGHTS = 4;

struct Light
{
	vec3 Position;
	vec3 Colour;
	float Radius;
};

struct DirectionalLight
{
	vec3 Direction;
	vec3 Colour;
};

uniform int lightCount = 0;
uniform Light lights[MAX_LIGHTS];
uniform vec3 ambientLighting = vec3(0.1, 0.1, 0.1);

vec3 BlinnPhong(vec3 albedo, vec3 fragPosition, vec3 normals)
{
	normals = normalize(normals);
	vec3 lighting = albedo * ambientLighting;
	vec3 viewDir = normalize(camera.Position - fragPosition);
	for(int i = 0; i < lightCount; i++)
	{
		float distance = length(lights[i].Position - fragPosition);
		if(distance >= lights[i].Radius)
			continue;
			
		vec3 lightDir = normalize(lights[i].Position - fragPosition);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normals, halfwayDir), 0.0), 64.0);

		vec3 diffuse = max(dot(normals, lightDir), 0.0) * albedo;

		lighting += diffuse + spec * lights[i].Colour;
	}
	return lighting;
}