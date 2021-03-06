#pragma once
#include <Engine/Application.hpp>

class ClothDemo : public Engine::Application
{
	void OnStart() override;
	void OnShutdown() override;

	void OnDraw() override;
	void OnUpdate() override;

public:
	ClothDemo(Engine::ApplicationArgs args);
};
