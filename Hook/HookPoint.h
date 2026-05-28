#pragma once

#include<Hook/Hook.h>
#include<Core/PassKey/PassKey.h>
#include<Core/Container/SlotMap.h>
#include<Log/Common/LogLevel.h>
#include<Engine/API/def.h>

namespace Pitaya::Engine { class Engine; }
namespace Pitaya::Render { class Renderer; class RenderPipeline; }
namespace Pitaya::GPU { struct FrameBuffer; }

namespace Pitaya::Engine
{
	//Lifecycle
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreBeginFrame_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreFixedUpdate_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreUpdate_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreLateUpdate_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreEndFrame_Hook;

	//Renderer
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(void*)> Global_PostRendererInitialize_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PostRendererRelease_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PostRendererParseCommand_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PostRendererSwapBuffer_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PostRendererBeginRenderFrame_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreRendereEndRenderFrame_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle)> Global_PostRenderContextInitialized_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PreRenderContextRelease_Hook;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::RenderPipeline*)> Global_PreRenderPipelineExecute_Hook;
	extern "C" ENGINE_API extern Hook<bool(ENGINE_CALL)()> Global_ShouldWakeupRenderThread_Hook;
	extern "C" ENGINE_API extern Hook<bool(ENGINE_CALL)()> Global_ShouldSubmitSceneCameraPass_Hook;
	extern "C" ENGINE_API extern Hook<bool(ENGINE_CALL)()> Global_TerminateRenderPipelineSubmitFinalBlit_Hook;

	//Physics
	extern "C" ENGINE_API extern Hook<bool(ENGINE_CALL)()> Global_TerminateFixedUpdate_Hook;

	//Time
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Global_PostChronometerTick_Hook;
	
	//Log
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Log::LogLevel, std::string_view)> Global_PostLog_Hook;
}
