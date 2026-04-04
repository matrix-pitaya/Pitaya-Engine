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
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreBeginFrame;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreFixedUpdate;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreUpdate;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreLateUpdate;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreEndFrame;
							
	//Renderer
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(void*)> Hook_PostRendererInitialize;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererRelease;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererParseCommand;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererSwapBuffer;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRendererBeginRenderFrame;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreRendereEndRenderFrame;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostRenderContextInitialized;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PreRenderContextRelease;
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, Pitaya::Render::RenderPipeline*)> Hook_PreRenderPipelineExecute;
	extern "C" ENGINE_API extern Hook<bool(ENGINE_CALL)()> Hook_ShouldWakeupRenderThread;

	//Time
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)()> Hook_PostChronometerTick;
	
	//Log
	extern "C" ENGINE_API extern Hook<void(ENGINE_CALL)(Pitaya::Log::LogLevel, std::string_view)> Hook_PostLog;
}

