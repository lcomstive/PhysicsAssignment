#pragma once
#include <Engine/Application.hpp>
#include <Engine/Physics/Shapes.hpp>

class SpringDemo : public Engine::Application
{
	void OnStart() override;
	void OnShutdown() override;

	void OnDraw() override;
	void OnUpdate() override;
	void OnDrawGizmos() override;

public:
	SpringDemo(Engine::ApplicationArgs args);
};
