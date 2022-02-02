#include <Engine/Graphics/Mesh.hpp>
#include <Engine/Graphics/Gizmos.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Components/Camera.hpp>
#include <Engine/Components/Transform.hpp>

using namespace std;
using namespace glm;
using namespace Engine::Physics;
using namespace Engine::Graphics;
using namespace Engine::Components;

vec4 Gizmos::Colour = { 1, 1, 1, 1 };
Shader* Gizmos::s_Shader = nullptr;
Material Gizmos::s_Material = {};

void Gizmos::DrawQuad(vec3 position, vec2 scale, vec3 rotation)
{
	s_Material.Albedo = Colour;
	s_Material.Wireframe = false;
	Renderer::Submit(DrawCall{ Mesh::Quad(), s_Material, position, { scale.x, scale.y, 1 }, rotation });
}

void Gizmos::DrawCube(vec3 position, vec3 scale, vec3 rotation)
{
	s_Material.Albedo = Colour;
	s_Material.Wireframe = false;
	Renderer::Submit(DrawCall{ Mesh::Cube(), s_Material, position, scale, rotation });
}

void Gizmos::DrawSphere(vec3 position, float radius)
{
	s_Material.Albedo = Colour;
	s_Material.Wireframe = false;
	Renderer::Submit(DrawCall{ Mesh::Sphere(), s_Material, position, { radius, radius, radius }, { 0, 0, 0 } });
}

void Gizmos::DrawWireQuad(vec3 position, vec2 scale, vec3 rotation)
{
	s_Material.Albedo = Colour;
	s_Material.Wireframe = true;
	Renderer::Submit(DrawCall{ Mesh::Quad(), s_Material, position, { scale.x, scale.y, 1 }, rotation });
}

void Gizmos::DrawWireCube(vec3 position, vec3 scale, vec3 rotation)
{
	s_Material.Albedo = Colour;
	s_Material.Wireframe = true;
	Renderer::Submit(DrawCall{ Mesh::Cube(), s_Material, position, scale, rotation });
}

void Gizmos::DrawWireSphere(vec3 position, float radius)
{
	s_Material.Albedo = Colour;
	s_Material.Wireframe = true;
	Renderer::Submit(DrawCall{ Mesh::Sphere(), s_Material, position, { radius, radius, radius }, { 0, 0, 0 } });
}

void Gizmos::DrawLine(vec3 start, vec3 end)
{
	static Mesh* LineMesh = nullptr;

	if (!LineMesh)
	{
		LineMesh = new Mesh({
			Mesh::Vertex { { 0, 0, 0 } },
			Mesh::Vertex { { 0, 0, 1 } },
			},
			{ 0, 1 },
			Mesh::DrawMode::Lines
		);
	}

	//// NOTE: DOESN'T WORK RELIABLY

	vec3 rotation = eulerAngles(quat(lookAt(start, end, { 0, 1, 0 })));

	s_Material.Albedo = Colour;
	s_Material.Wireframe = true;
	Renderer::Submit(DrawCall{ LineMesh, s_Material, start, { 0, 0, length(end - start) }, rotation });
}

void Gizmos::DrawLine(Line line) { DrawLine(line.Start, line.End); }
void Gizmos::DrawRay(Ray ray) { DrawLine(ray.Origin, ray.Origin + ray.Direction * 1000.0f); }
