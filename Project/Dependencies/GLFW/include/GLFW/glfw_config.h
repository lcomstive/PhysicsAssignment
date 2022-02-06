#if _WIN32
	#define _GLFW_WIN32
	#define USE_MSVC_RUNTIME_LIBRARY_DLL
#elif __APPLE__
	#define _GLFW_COCOA
#endif
