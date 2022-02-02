#pragma once
#include <string>
#include <vector>

namespace Engine
{
	std::string ReadText(std::string path);
	std::vector<unsigned char> Read(std::string path);
}