#include "PhysicsDemo.hpp"

using namespace std::chrono_literals; // seconds in literal

Engine::ApplicationArgs applicationArgs =
{
	false,			// VSync
	1,				// Samples
	"Physics Demo",	// Title
	{ 1280, 720 },	// Resolution
	50ms			// Physics Timestep
};

int main()
{
	PhysicsDemo demo(applicationArgs);
	demo.Run();
	return 0;
}