#pragma once

#if defined(PITAYA_DLL_BUILD) 
	#if defined(PITAYA_PLATFORM_WINDOWS)
		#ifdef ENGINE_EXPORT
			#define ENGINE_API __declspec(dllexport) 
		#else
			#define ENGINE_API __declspec(dllimport) 
		#endif
	#elif defined(PITAYA_PLATFORM_LINUX) || defined(PITAYA_PLATFORM_MAC) || defined(PITAYA_PLATFORM_ANDROID) || defined(PITAYA_PLATFORM_IOS)
		#ifdef ENGINE_EXPORT
			#define ENGINE_API __attribute__((visibility("default")))
		#else
			#define ENGINE_API 
		#endif
	#else
		#define ENGINE_API 
	#endif
#else
	#define ENGINE_API
#endif

#if defined(PITAYA_INTEROP_MODE) 
	#if defined(PITAYA_PLATFORM_WINDOWS)
		#define ENGINE_CALL __stdcall
	#else
		#define ENGINE_CALL 
	#endif
#else
	#if defined(PITAYA_PLATFORM_WINDOWS)
		#define ENGINE_CALL __cdecl
	#else
		#define ENGINE_CALL 
	#endif
#endif
