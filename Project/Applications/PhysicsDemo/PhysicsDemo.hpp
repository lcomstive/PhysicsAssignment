#pragma once
#include <Engine/Application.hpp>

class PhysicsDemo : public Engine::Application
{
	void OnStart() override;
	void OnShutdown() override;

	void OnDraw() override;
	void OnUpdate() override;
	void OnDrawGizmos() override;

public:
	PhysicsDemo(Engine::ApplicationArgs args);
};
