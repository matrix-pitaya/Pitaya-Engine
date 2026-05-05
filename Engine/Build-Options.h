#pragma once

// Pitaya Engine Version
#define PITAYA_VERSION "dev-1.0.0"

// Enable Pitaya Console
#define PITAYA_CONSOLE

// Graphics Render Backend
#define PITAYA_GRAPHICS_OPENGL

// Window Backend
#define PITAYA_WINDOW_GLFW

// Physics Simulator Backend
#define PITAYA_PHYSICS_BULLET

// Platform
#if defined(_WIN32) || defined(_WIN64)
	#define PITAYA_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
	#define PITAYA_PLATFORM_ANDROID
#elif defined(__linux__)
	#define PITAYA_PLATFORM_LINUX
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define PITAYA_PLATFORM_IOS
	#elif TARGET_OS_MAC
		#define PITAYA_PLATFORM_MAC
	#else
		#error "Pitaya Engine: Unknown Apple platform!"
	#endif
#else
	#error "Pitaya Engine: Unsupported Platform!"
#endif

// Build Project
#if defined(PITAYA_PROJECT_ENGINE)
	#define PITAYA_DLL_BUILD
	#define ENGINE_EXPORT
#elif defined(PITAYA_PROJECT_EDITOR)
	#define PITAYA_DLL_BUILD
	#define EDITOR_EXPORT
#elif defined(PITAYA_PROJECT_APPLICATION)
	#define PITAYA_DLL_BUILD
#else
	#error "Pitaya Engine: Unsupported Build Project!"
#endif
