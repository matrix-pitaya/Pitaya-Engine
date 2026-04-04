#pragma once

#include<Hook/Hook.h>
#include<Core/PassKey/PassKey.h>
#include<Log/Common/LogLevel.h>
#include<Engine/API/def.h>

namespace Pitaya::Engine { class Engine; }
namespace Pitaya::Render { class RenderPipeline; }

namespace Pitaya::Engine
{
	//Lifecycle
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_BeginFrame;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_FixedUpdate;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_Update;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_LateUpdate;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_EndFrame;
							
	//Renderer
	ENGINE_API extern Hook<void(ENGINE_CALL)(void*)> Hook_PostRendererInitialize;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererRelease;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererParseCommand;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererSwapBuffer;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererBeginRenderFrame;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreRendereEndRenderFrame;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRenderContextInitialized;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreRenderContextRelease;
	ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::RenderPipeline*)> Hook_PreRenderPipelineExecute;
	ENGINE_API extern Hook<bool(ENGINE_CALL)()> Hook_ShouldWakeupRenderThread;

	//Time
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostChronometerTick;
	
	//Log
	ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Log::LogLevel, std::string_view)> Hook_PostLog;
}

