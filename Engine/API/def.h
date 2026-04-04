#pragma once

#if defined(PITAYA_DLL_BUILD)
	#ifdef ENGINE_EXPORT
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#else
	#define ENGINE_API
#endif


#if defined(PITAYA_INTEROP_MODE)
	#define ENGINE_CALL __stdcall
#else
	#define ENGINE_CALL __cdecl
#endif
