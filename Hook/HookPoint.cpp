#include<Hook/HookPoint.h>

//Lifecycle
void (ENGINE_CALL *Pitaya::Engine::HookFunc_BeginFrame)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_FixedUpdate)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_Update)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_LateUpdate)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_EndFrame)() = nullptr;
			    
//Renderer	    
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PostRendererInitialize)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PostRendererRelease)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PostRendererParseCommand)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PostRendererSwapBuffer)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PostRenderContextInitialized)() = nullptr;
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PreRenderContextRelease)() = nullptr;
bool (ENGINE_CALL *Pitaya::Engine::HookFunc_ShouldWakeupRenderThread)() = nullptr;

//Time		    
void (ENGINE_CALL *Pitaya::Engine::HookFunc_PostChronometerTick)() = nullptr;