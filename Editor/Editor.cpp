#include<Editor/Editor.h>
#include<Event/Common/FuncTable.h>
#include<Render/RenderPipeline.h>
#include<Physics/Frontend/PhysicsSimulator.h>
#include<Time/Common/FuncTable.h> 
#include<Core/Utils/System.h>
#include<Core/Utils/Check.h>
#include<Hook/def.h>

extern "C" EDITOR_API void EDITOR_CALL AttachRuntimeEnv(int argc, char** argv)
{
	Pitaya::Editor::Editor::AttachRuntimeEnv(argc,argv);
}

void Pitaya::Editor::Editor::HookFunc::PreBeginFrame()
{
	Pitaya::Editor::Editor::Instance().BeginFrame();
}
void Pitaya::Editor::Editor::HookFunc::PreFixedUpdate()
{
	Pitaya::Editor::Editor::Instance().FixedUpdate();
}
void Pitaya::Editor::Editor::HookFunc::PreUpdate()
{
	Pitaya::Editor::Editor::Instance().Updata();
}
void Pitaya::Editor::Editor::HookFunc::PreLateUpdate()
{
	Pitaya::Editor::Editor::Instance().LateUpdate();
}
void Pitaya::Editor::Editor::HookFunc::PreEndFrame()
{
	Pitaya::Editor::Editor::Instance().EndFrame();
}
void Pitaya::Editor::Editor::HookFunc::PostRendererIntialize(void* nativeWindow)
{
	if (!Pitaya::Editor::Editor::Instance().Initialize_Main(nativeWindow))
	{
		throw std::runtime_error("Editor Main Initialzie Fail!");
	}
}
void Pitaya::Editor::Editor::HookFunc::PostRendererRelease()
{
	Pitaya::Editor::Editor::Instance().Release_Main();
}
void Pitaya::Editor::Editor::HookFunc::PostRenderContextInitialized(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle rtHandle)
{
	PITAYA_CHECK(Pitaya::Editor::Editor::Instance().Initialize_Render(rtHandle))
}
void Pitaya::Editor::Editor::HookFunc::PreRenderContextReleased()
{
	Pitaya::Editor::Editor::Instance().Release_Render();
}
void Pitaya::Editor::Editor::HookFunc::PostRendererSwapBuffer()
{
	Pitaya::Editor::Editor::Instance().gui.drawer.SwapBuffer(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
}
void Pitaya::Editor::Editor::HookFunc::PreRendererEndRenderFrame()
{
	Pitaya::Editor::Editor::Instance().gui.drawer.CreateFrontDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
}
void Pitaya::Editor::Editor::HookFunc::PostRendererBeginRenderFrame()
{
	Pitaya::Editor::Editor::Instance().gui.drawer.ReleaseFrontDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
}
bool Pitaya::Editor::Editor::HookFunc::ShouldWakeupRenderThread()
{
	return Pitaya::Editor::Editor::Instance().gui.drawer.HasRenderDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
}
bool Pitaya::Editor::Editor::HookFunc::ShouldSubmitSceneCameraPass()
{
	return Pitaya::Editor::Editor::Instance().gui.panels.gameViewportPanel.GetIsVisable();
}
void Pitaya::Editor::Editor::HookFunc::PreRenderPipelineExecute(Pitaya::Core::PassKey<Pitaya::Engine::Engine> passkey, Pitaya::Render::RenderPipeline* renderPipeline)
{
	auto& editor = Pitaya::Editor::Editor::Instance();

	//提交EditorCamera
	if (editor.gui.panels.sceneViewportPanel.GetIsVisable())
	{
		//TODO 增加特殊网格对象
		if (editor.gui.context.GizmoState.ShowGrid)
		{
			//renderPipeline->AddRenderObject();
		}
		
		renderPipeline->AddRenderPass(passkey,
			editor.camera.GetCameraSnapshot(), editor.camera.GetPostProcessSettings(),
			editor.camera.GetCullingMask(), editor.camera.GetNativeRenderTarget(),
			editor.camera.GetCameraState().NearClip, editor.camera.GetCameraState().FarClip);
	}

	//提交UI
	editor.gui.NewFrame();
}
void Pitaya::Editor::Editor::HookFunc::PostRendererParseCommand()
{
	auto& editor = Pitaya::Editor::Editor::Instance();
	editor.gui.drawer.Draw(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
	editor.gui.drawer.ReleaseBackDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
}
void Pitaya::Editor::Editor::HookFunc::PostChronometerTick()
{
	auto& editor = Pitaya::Editor::Editor::Instance();
	if (editor.gui.panels.profilerPanel.GetIsVisable())
	{
		editor.profiler.SetTimeState({ Pitaya::Time::delta(), Pitaya::Physics::PhysicsSimulator::FixedTimestep, Pitaya::Time::UnscaledDelta(),
			Pitaya::Time::TimeScale(), Pitaya::Time::Framerate(), Pitaya::Time::Seconds(), Pitaya::Time::Milliseconds() });
	}
}
void Pitaya::Editor::Editor::HookFunc::PostLog(Pitaya::Log::LogLevel level, std::string_view message)
{
	if (level != Pitaya::Log::LogLevel::Info)
	{
		Pitaya::Editor::Editor::Instance().gui.panels.consolePanel.Console(level, message);
	}
}
bool Pitaya::Editor::Editor::HookFunc::ShouldPhysicsStep()
{
	auto& editor = Pitaya::Editor::Editor::Instance();
	return editor.stateMachine.GetCurrentState() == Pitaya::Editor::EngineState::Play && 
		!editor.gui.context.ToolState.IsPaused;
}
bool Pitaya::Editor::Editor::HookFunc::TerminateRenderPipelineSubmitFinalBlit()
{
	return true;	//只要有Editor.dll挂载 就应该将MainDisplayRT渲染到Imgui 再由Imgui渲染到0号缓冲区
}

bool Pitaya::Editor::Editor::Initialize_Main(void* nativeWindow)
{
	mouseScrollToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::MouseScroll,
		&Pitaya::Editor::Editor::OnMouseScroll, this);
	
	mouseCurrsorMoveToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::MouseCurrsorMove,
		&Pitaya::Editor::Editor::OnMouseCurrsorMove, this);
	
	return camera.Initialize_Main() && gui.Initialize_Main(nativeWindow);
}
bool Pitaya::Editor::Editor::Initialize_Render(Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle rtHandle)
{
	return camera.Initialize_Render() && gui.Initialize_Render(rtHandle, camera.renderTarget.FinalFrameBufferHandle);
}
void Pitaya::Editor::Editor::Release_Main()
{
	Pitaya::Event::UnSubscribe(mouseScrollToken);
	Pitaya::Event::UnSubscribe(mouseCurrsorMoveToken);
	camera.Release_Main();
	gui.Release_Main();
}
void Pitaya::Editor::Editor::Release_Render()
{
	camera.Release_Render();
	gui.Release_Render();
}
void Pitaya::Editor::Editor::BeginFrame()
{
	profiler.BeginFrame();
}
void Pitaya::Editor::Editor::FixedUpdate()
{

}
void Pitaya::Editor::Editor::Updata()
{
	camera.Update();
}
void Pitaya::Editor::Editor::LateUpdate()
{

}
void Pitaya::Editor::Editor::EndFrame()
{
	profiler.EndFrame();
}
void Pitaya::Editor::Editor::OnMouseScroll(Pitaya::Event::Event event)
{
	if (gui.panels.sceneViewportPanel.GetIsFocused())
	{
		camera.OnMouseScroll(event.args.mouseScroll);
	}
}
void Pitaya::Editor::Editor::OnMouseCurrsorMove(Pitaya::Event::Event event)
{
	if (gui.panels.sceneViewportPanel.GetIsFocused())
	{
		camera.OnMouseCurrsorMove(event.args.mouseCurrsor);
	}
}

void Pitaya::Editor::Editor::AttachRuntimeEnv(int argc, char** argv)
{
	MOUNT_PREBEGINFRAME_HOOK(Pitaya::Editor::Editor::HookFunc::PreBeginFrame, "Editor::BeginFrame")
	MOUNT_PREFIXEDUPDATE_HOOK(Pitaya::Editor::Editor::HookFunc::PreFixedUpdate, "Editor::FixUpdate")
	MOUNT_PREUPDATE_HOOK(Pitaya::Editor::Editor::HookFunc::PreUpdate, "Editor::Update")
	MOUNT_PRELATEUPDATE_HOOK(Pitaya::Editor::Editor::HookFunc::PreLateUpdate, "Editor::LateUpdate")
	MOUNT_PREENDFRAME_HOOK(Pitaya::Editor::Editor::HookFunc::PreEndFrame, "Editor::EndFrame")
	MOUNT_POSTRENDERERINTIALIZE_HOOK(Pitaya::Editor::Editor::HookFunc::PostRendererIntialize, "Editor::Initialize")
	MOUNT_POSTRENDERERRELEASE_HOOK(Pitaya::Editor::Editor::HookFunc::PostRendererRelease, "Editor::Release")
	MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK(Pitaya::Editor::Editor::HookFunc::PostRenderContextInitialized, "Editor::InitializeForRender")
	MOUNT_PRERENDERCONTEXTINRELEASED_HOOK(Pitaya::Editor::Editor::HookFunc::PreRenderContextReleased, "Editor::ReleaseForRender")
	MOUNT_POSTRENDERERSWAPBUFFER_HOOK(Pitaya::Editor::Editor::HookFunc::PostRendererSwapBuffer, "Editor::GUI::SwapBuffer")
	MOUNT_PRERENDERERENDRENDERFRAME_HOOK(Pitaya::Editor::Editor::HookFunc::PreRendererEndRenderFrame, "Editor::GUI::CreateFrontDrawData")
	MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK(Pitaya::Editor::Editor::HookFunc::PostRendererBeginRenderFrame, "Editor::GUI::ReleaseFrontDrawData")
	MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK(Pitaya::Editor::Editor::HookFunc::ShouldWakeupRenderThread, "Editor::GUI::HasRenderDrawData")
	MOUNT_SHOULDSUBMITSCENECAMERAPASS_HOOK(Pitaya::Editor::Editor::HookFunc::ShouldSubmitSceneCameraPass, "Editor::GUI::GetGameViewportPanelIsVisable")
	MOUNT_PRERENDERPIPELINEEXECUTE_HOOK(Pitaya::Editor::Editor::HookFunc::PreRenderPipelineExecute, "Editor::SubmitCameraPass | Editor::GUI::NewFrame")
	MOUNT_POSTRENDERERPARSECOMMAND_HOOK(Pitaya::Editor::Editor::HookFunc::PostRendererParseCommand, "Editor::GUI::Draw | Editor::GUI::ReleaseBackDrawData")
	MOUNT_POSTCHRONOMETERTICK_HOOK(Pitaya::Editor::Editor::HookFunc::PostChronometerTick, "Editor::Profiler::UploadTimeState")
	MOUNT_POSTLOG_HOOK(Pitaya::Editor::Editor::HookFunc::PostLog, "Editor::GUI::Console")
	MOUNT_SHOULDPHYSICSSTEP_HOOK(Pitaya::Editor::Editor::HookFunc::ShouldPhysicsStep, "Editor::StateMachine::IsPlaying")
	MOUNT_TERMINATERENDERPIPELINESUBMITFINALBLIT_HOOK(Pitaya::Editor::Editor::HookFunc::TerminateRenderPipelineSubmitFinalBlit,"Editor::TerminateRenderPipelineSubmitFinalBlit")
}

template<>
Pitaya::Editor::Editor& EDITOR_CALL Pitaya::Core::Singleton<Pitaya::Editor::Editor>::Instance()
{
	static Pitaya::Editor::Editor instance;
	return instance;
}
