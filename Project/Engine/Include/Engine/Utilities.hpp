#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Engine
{
	std::string ReadText(std::string path);
	std::vector<unsigned char> Read(std::string path);

	float Magnitude(glm::vec3 vector);
	float MagnitudeSqr(glm::vec3 vector);

	/// <summary>
	/// Yeeeaaahhh it's close enough
	/// </summary>
	bool BasicallyZero(float f);

	float Random(float min, float max);
}