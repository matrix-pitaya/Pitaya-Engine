#pragma once

#include<Engine/API/def.h>

namespace Pitaya::Engine
{
	//Lifecycle
	ENGINE_API extern void (ENGINE_CALL *HookFunc_BeginFrame)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_FixedUpdate)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_Update)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_LateUpdate)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_EndFrame)();
							
	//Renderer				 
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PostRendererInitialize)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PostRendererRelease)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PostRendererParseCommand)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PostRendererSwapBuffer)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PostRenderContextInitialized)();
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PreRenderContextRelease)();
	ENGINE_API extern bool (ENGINE_CALL *HookFunc_ShouldWakeupRenderThread)();

	//Time					
	ENGINE_API extern void (ENGINE_CALL *HookFunc_PostChronometerTick)();
}

