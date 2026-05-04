#include<Hook/HookPoint.h>

//Lifecycle
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreBeginFrame_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreFixedUpdate_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreUpdate_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreLateUpdate_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreEndFrame_Hook;
			    
//Renderer
Pitaya::Engine::Hook<void(ENGINE_CALL)(void*)> Pitaya::Engine::Global_PostRendererInitialize_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PostRendererRelease_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PostRendererParseCommand_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PostRendererSwapBuffer_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)(Pitaya::Core::PassKey<Pitaya::Render::Renderer>, Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle)> Pitaya::Engine::Global_PostRenderContextInitialized_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PreRenderContextRelease_Hook;
Pitaya::Engine::Hook<void(ENGINE_CALL)(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::RenderPipeline*)> Pitaya::Engine::Global_PreRenderPipelineExecute_Hook;
Pitaya::Engine::Hook<bool(ENGINE_CALL)()> Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook;
Pitaya::Engine::Hook<bool(ENGINE_CALL)()> Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook;
Pitaya::Engine::Hook<bool(ENGINE_CALL)()> Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook;

//Physics
Pitaya::Engine::Hook<bool(ENGINE_CALL)()> Pitaya::Engine::Global_TerminateFixedUpdate_Hook;

//Time		    
Pitaya::Engine::Hook<void(ENGINE_CALL)()> Pitaya::Engine::Global_PostChronometerTick_Hook;

//Log
Pitaya::Engine::Hook<void(ENGINE_CALL)(Pitaya::Log::LogLevel, std::string_view)> Pitaya::Engine::Global_PostLog_Hook;
