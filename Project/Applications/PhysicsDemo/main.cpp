#include "PhysicsDemo.hpp"

using namespace std::chrono_literals; // seconds in literal

Engine::ApplicationArgs applicationArgs =
{
	false,			// VSync
	4,				// Samples
	"Physics Demo",	// Title
	{ 1280, 720 },	// Resolution
	20ms			// Physics Timestep
};

#ifndef _WIN32
int main()
#else
int __stdcall WinMain(void*, void*, char*, int)
#endif
{
	PhysicsDemo demo(applicationArgs);
	demo.Run();
	return 0;
}
