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
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreBeginFrame;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreFixedUpdate;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreUpdate;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreLateUpdate;
	ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreEndFrame;
							
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

