#pragma once

#if defined(PITAYA_DLL_BUILD)
	#ifdef EDITOR_EXPORT
		#define EDITOR_API __declspec(dllexport)
	#else
		#define EDITOR_API __declspec(dllimport)
	#endif
#else
	#define EDITOR_API
#endif


#if defined(PITAYA_INTEROP_MODE)
	#define EDITOR_CALL __stdcall
#else
	#define EDITOR_CALL __cdecl
#endif
