#include<Editor/Editor.h>
#include<Event/Common/FuncTable.h>

bool Pitaya::Editor::Editor::Initialize(void* nativeWindow)
{
	mouseScrollToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::MouseScroll,
		&Pitaya::Editor::Editor::OnMouseScroll, this);
	
	mouseCurrsorMoveToken = Pitaya::Event::Subscribe(
		Pitaya::Event::EventType::MouseCurrsorMove,
		&Pitaya::Editor::Editor::OnMouseCurrsorMove, this);
	
	return camera.Initialize() && gui.InitializeForMain(nativeWindow);
}
void Pitaya::Editor::Editor::Release()
{
	Pitaya::Event::UnSubscribe(mouseScrollToken);
	Pitaya::Event::UnSubscribe(mouseCurrsorMoveToken);
	camera.Release();
	gui.ReleaseForMain();
}
bool Pitaya::Editor::Editor::InitializeForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer>)
{
	return gui.InitializeForRender();
}
void Pitaya::Editor::Editor::ReleaseForRender(Pitaya::Core::PassKey<Pitaya::Render::Renderer>)
{
	gui.ReleaseForRender();
}
void Pitaya::Editor::Editor::Console(Pitaya::Log::LogLevel level, std::string_view message)
{
	gui.consolePanel.Console(level, message);
}
void Pitaya::Editor::Editor::Updata()
{
	if (gui.sceneViewportPanel.GetIsFocused()) { camera.Updata(); }
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
