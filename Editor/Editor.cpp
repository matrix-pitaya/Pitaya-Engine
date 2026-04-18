#include<Editor/Editor.h>
#include<Event/Common/FuncTable.h>
#include<Render/RenderPipeline.h>
#include<Hook/def.h>
#include<Time/Common/FuncTable.h> 

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
	Pitaya::Editor::Editor::Instance().Initialize_Main(nativeWindow);
}
void Pitaya::Editor::Editor::HookFunc::PostRendererRelease()
{
	Pitaya::Editor::Editor::Instance().Release_Main();
}
void Pitaya::Editor::Editor::HookFunc::PostRenderContextInitialized()
{
	Pitaya::Editor::Editor::Instance().Initialize_Render();
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
void Pitaya::Editor::Editor::HookFunc::PreRenderPipelineExecute(Pitaya::Core::PassKey<Pitaya::Engine::Engine> passkey, Pitaya::Render::RenderPipeline* renderPipeline)
{
	//提交EditorCamera
	if (Pitaya::Editor::Editor::Instance().gui.panels.sceneViewportPanel.GetIsVisable() && 
		Pitaya::Editor::Editor::Instance().camera.GetRenderTargetIsReady())
	{
		//TODO 增加特殊网格对象 天空盒对象
		if (Pitaya::Editor::Editor::Instance().gui.context.GizmoState.ShowGrid)
		{
			//renderPipeline->AddRenderObject();
		}
		
		renderPipeline->AddRenderPass(passkey,
			Pitaya::Editor::Editor::Instance().camera.GetCameraSnapshot(), Pitaya::Editor::Editor::Instance().camera.GetRenderTarget(),
			Pitaya::Editor::Editor::Instance().camera.GetPostProcessSettings(), Pitaya::Editor::Editor::Instance().camera.GetCullingMask());
	}

	//提交UI
	Pitaya::Editor::Editor::Instance().gui.NewFrame();
}
void Pitaya::Editor::Editor::HookFunc::PostRendererParseCommand()
{
	Pitaya::Editor::Editor::Instance().gui.drawer.Draw(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
	Pitaya::Editor::Editor::Instance().gui.drawer.ReleaseBackDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
}
void Pitaya::Editor::Editor::HookFunc::PostChronometerTick()
{
	if (Pitaya::Editor::Editor::Instance().gui.panels.profilerPanel.GetIsVisable())
	{
		Pitaya::Editor::Editor::Instance().profiler.SetTimeState({ Pitaya::Time::delta() ,Pitaya::Time::Fixdelta() ,
			Pitaya::Time::UnscaledDelta() , Pitaya::Time::TimeScale(), Pitaya::Time::Framerate() ,
			Pitaya::Time::Seconds() , Pitaya::Time::Milliseconds() });
	}
}
void Pitaya::Editor::Editor::HookFunc::PostLog(Pitaya::Log::LogLevel level, std::string_view message)
{
	if (level != Pitaya::Log::LogLevel::Info)
	{
		Pitaya::Editor::Editor::Instance().gui.panels.consolePanel.Console(level, message);
	}
}

bool Pitaya::Editor::Editor::Initialize_Main(void* nativeWindow)
{
	mouseScrollToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::MouseScroll,
		&Pitaya::Editor::Editor::OnMouseScroll, this);
	
	mouseCurrsorMoveToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::MouseCurrsorMove,
		&Pitaya::Editor::Editor::OnMouseCurrsorMove, this);
	
	return camera.Initialize() && gui.Initialize_Main(nativeWindow);
}
bool Pitaya::Editor::Editor::Initialize_Render()
{
	return gui.Initialize_Render();
}
void Pitaya::Editor::Editor::Release_Main()
{
	Pitaya::Event::UnSubscribe(mouseScrollToken);
	Pitaya::Event::UnSubscribe(mouseCurrsorMoveToken);
	camera.Release();
	gui.Release_Main();
}
void Pitaya::Editor::Editor::Release_Render()
{
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
void Pitaya::Editor::Editor::OnMouseScroll(const Pitaya::Event::Event& event)
{
	if (event.type != Pitaya::Event::EventType::MouseScroll) { return; }
	const Pitaya::Event::MouseScrollEventArgs& args = static_cast<const Pitaya::Event::MouseScrollEventArgs&>(event.args);
	if (gui.panels.sceneViewportPanel.GetIsFocused()) { camera.OnMouseScroll(args); }
}
void Pitaya::Editor::Editor::OnMouseCurrsorMove(const Pitaya::Event::Event& event)
{
	if (event.type != Pitaya::Event::EventType::MouseCurrsorMove) { return; }
	const Pitaya::Event::MouseCurrsorMoveEventArgs& args = static_cast<const Pitaya::Event::MouseCurrsorMoveEventArgs&>(event.args);
	if (gui.panels.sceneViewportPanel.GetIsFocused()) { camera.OnMouseCurrsorMove(args); }
}

void Pitaya::Editor::Editor::MountEngineHook()
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
	MOUNT_PRERENDERPIPELINEEXECUTE_HOOK(Pitaya::Editor::Editor::HookFunc::PreRenderPipelineExecute, "Editor::SubmitCameraPass | Editor::GUI::NewFrame")
	MOUNT_POSTRENDERERPARSECOMMAND_HOOK(Pitaya::Editor::Editor::HookFunc::PostRendererParseCommand, "Editor::GUI::Draw | Editor::GUI::ReleaseBackDrawData")
	MOUNT_POSTCHRONOMETERTICK_HOOK(Pitaya::Editor::Editor::HookFunc::PostChronometerTick, "Editor::Profiler::UploadTimeState")
	MOUNT_POSTLOG_HOOK(Pitaya::Editor::Editor::HookFunc::PostLog, "Editor::GUI::Console")
}

template<>
Pitaya::Editor::Editor& EDITOR_CALL Pitaya::Core::Singleton<Pitaya::Editor::Editor>::Instance()
{
	static Pitaya::Editor::Editor instance;
	return instance;
}
