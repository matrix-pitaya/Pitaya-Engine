#include<Editor/Editor.h>
#include<Event/Common/FuncTable.h>
#include<Hook/def.h>
#include<Render/RenderPipeline.h>

#include<Time/Common/FuncTable.h> 

void Pitaya::Editor::Editor::MountEngineHook()
{
	MOUNT_PREBEGINFRAME_HOOK([]() { Pitaya::Editor::Editor::Instance().BeginFrame(); }, "Editor::BeginFrame")
	MOUNT_PREFIXEDUPDATE_HOOK([]() { Pitaya::Editor::Editor::Instance().FixedUpdate(); }, "Editor::FixUpdate")
	MOUNT_PREUPDATE_HOOK([]() { Pitaya::Editor::Editor::Instance().Updata(); }, "Editor::Update")
	MOUNT_PRELATEUPDATE_HOOK([]() { Pitaya::Editor::Editor::Instance().LateUpdate(); }, "Editor::LateUpdate")
	MOUNT_PREENDFRAME_HOOK([]() { Pitaya::Editor::Editor::Instance().EndFrame(); }, "Editor::EndFrame")
	MOUNT_POSTRENDERERINTIALIZE_HOOK([](void* nativeWindow) {Pitaya::Editor::Editor::Instance().Initialize_Main(nativeWindow); }, "Editor::Initialize")
	MOUNT_POSTRENDERERRELEASE_HOOK([]() { Pitaya::Editor::Editor::Instance().Release_Main(); }, "Editor::Release")
	MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK([]() { Pitaya::Editor::Editor::Instance().Initialize_Render(); }, "Editor::InitializeForRender")
	MOUNT_PRERENDERCONTEXTINRELEASED_HOOK([]() { Pitaya::Editor::Editor::Instance().Release_Render(); }, "Editor::ReleaseForRender")
	MOUNT_POSTRENDERERSWAPBUFFER_HOOK([]() {Pitaya::Editor::Editor::Instance().gui.drawer.SwapBuffer(Pitaya::Core::PassKey<Pitaya::Editor::Editor>()); }, "Editor::GUI::SwapBuffer")
	MOUNT_PRERENDERERENDRENDERFRAME_HOOK([]() { Pitaya::Editor::Editor::Instance().gui.drawer.CreateFrontDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>()); }, "Editor::GUI::CreateFrontDrawData")
	MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK([]() { Pitaya::Editor::Editor::Instance().gui.drawer.ReleaseFrontDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>()); }, "Editor::GUI::ReleaseFrontDrawData")
	MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK([]() -> bool { return Pitaya::Editor::Editor::Instance().gui.drawer.HasRenderDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>()); }, "Editor::GUI::HasRenderDrawData")
	MOUNT_PRERENDERPIPELINEEXECUTE_HOOK([](Pitaya::Core::PassKey<Pitaya::Engine::Engine> passkey, Pitaya::Render::RenderPipeline* renderPipeline)
		{
			//提交EditorCamera
			if (Pitaya::Editor::Editor::Instance().gui.sceneViewportPanel.GetIsVisable() && Pitaya::Editor::Editor::Instance().camera.IsRenderTargetReady())
			{
				//TODO 增加特殊网格对象 天空盒对象
				//renderPipeline->AddRenderObject();
				renderPipeline->AddRenderPass(passkey,
					Pitaya::Editor::Editor::Instance().camera.GetCameraSnapshot(), Pitaya::Editor::Editor::Instance().camera.GetRenderTarget(),
					Pitaya::Editor::Editor::Instance().camera.GetPostProcessSettings(), Pitaya::Editor::Editor::Instance().camera.GetCullingMask());
			}

			//提交UI
			Pitaya::Editor::Editor::Instance().gui.NewFrame();
		}, "Editor::SubmitCameraPass | Editor::GUI::NewFrame")
	MOUNT_POSTRENDERERPARSECOMMAND_HOOK([]()
	{
		Pitaya::Editor::Editor::Instance().gui.drawer.Draw(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
		Pitaya::Editor::Editor::Instance().gui.drawer.ReleaseBackDrawData(Pitaya::Core::PassKey<Pitaya::Editor::Editor>());
	}, "Editor::GUI::Draw | Editor::GUI::ReleaseBackDrawData")
	MOUNT_POSTCHRONOMETERTICK_HOOK([]()
		{
			Pitaya::Editor::Editor::Instance().profiler.SetTimeState({ Pitaya::Time::delta() ,Pitaya::Time::Fixdelta() ,
				Pitaya::Time::UnscaledDelta() , Pitaya::Time::TimeScale(),Pitaya::Time::Framerate() ,
				Pitaya::Time::Seconds() , Pitaya::Time::Milliseconds() });
		}, "Editor::Profiler::UploadTimeState")
	MOUNT_POSTLOG_HOOK([](Pitaya::Log::LogLevel level, std::string_view message)
		{
			Pitaya::Editor::Editor::Instance().gui.consolePanel.Console(level, message);
		}, "Editor::GUI::Console")
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
	if (gui.sceneViewportPanel.GetIsFocused()) { camera.Updata(); }
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
	if (gui.sceneViewportPanel.GetIsFocused()) { camera.OnMouseScroll(args); }
}
void Pitaya::Editor::Editor::OnMouseCurrsorMove(const Pitaya::Event::Event& event)
{
	if (event.type != Pitaya::Event::EventType::MouseCurrsorMove) { return; }
	const Pitaya::Event::MouseCurrsorMoveEventArgs& args = static_cast<const Pitaya::Event::MouseCurrsorMoveEventArgs&>(event.args);
	if (gui.sceneViewportPanel.GetIsFocused()) { camera.OnMouseCurrsorMove(args); }
}

template<>
Pitaya::Editor::Editor& EDITOR_CALL Pitaya::Core::Singleton<Pitaya::Editor::Editor>::Instance()
{
	static Pitaya::Editor::Editor instance;
	return instance;
}
