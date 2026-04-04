#include<Hook/HookPoint.h>

//Lifecycle
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_BeginFrame;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_FixedUpdate;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_Update;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_LateUpdate;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_EndFrame;
			    
//Renderer
Pitaya::Engine::Hook<void(ENGINE_CALL)(void*)> Pitaya::Engine::Hook_PostRendererInitialize;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PostRendererRelease;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PostRendererParseCommand;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PostRendererSwapBuffer;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PostRendererBeginRenderFrame;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PreRendereEndRenderFrame;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PostRenderContextInitialized;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PreRenderContextRelease;
Pitaya::Engine::Hook<void(ENGINE_CALL)(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::RenderPipeline*)> Pitaya::Engine::Hook_PreRenderPipelineExecute;
Pitaya::Engine::Hook<bool(ENGINE_CALL)()> Pitaya::Engine::Hook_ShouldWakeupRenderThread;

//Time		    
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Hook_PostChronometerTick;

//Log
Pitaya::Engine::Hook<void(ENGINE_CALL)(Pitaya::Log::LogLevel, std::string_view)> Pitaya::Engine::Hook_PostLog;
