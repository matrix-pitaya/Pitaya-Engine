#pragma once

#if defined(PITAYA_DLL_BUILD) 
	#if defined(PITAYA_PLATFORM_WINDOWS)
		#ifdef EDITOR_EXPORT
			#define EDITOR_API __declspec(dllexport) 
		#else
			#define EDITOR_API __declspec(dllimport) 
		#endif
	#elif defined(PITAYA_PLATFORM_LINUX) || defined(PITAYA_PLATFORM_MAC) || defined(PITAYA_PLATFORM_ANDROID) || defined(PITAYA_PLATFORM_IOS)
		#ifdef EDITOR_EXPORT
			#define EDITOR_API __attribute__((visibility("default")))
		#else
			#define EDITOR_API 
		#endif
	#else
		#define EDITOR_API 
	#endif
#else
	#define EDITOR_API
#endif

#if defined(PITAYA_INTEROP_MODE) 
	#if defined(PITAYA_PLATFORM_WINDOWS)
		#define EDITOR_CALL __stdcall
	#else
		#define EDITOR_CALL 
	#endif
#else
	#if defined(PITAYA_PLATFORM_WINDOWS)
		#define EDITOR_CALL __cdecl
	#else
		#define EDITOR_CALL 
	#endif
#endif
