#include<Engine/Internal/Input/Input.h>

#include<GLFW/glfw3.h>

bool Pitaya::Engine::Internal::Input::Initialize()
{
	keyEventToken = Pitaya::Engine::Event::Subscribe(Pitaya::Engine::Event::EventType::Key, std::bind(&Pitaya::Engine::Internal::Input::ProcessKeyEvent, this, std::placeholders::_1));
	mouseButtonEventToken = Pitaya::Engine::Event::Subscribe(Pitaya::Engine::Event::EventType::Key, std::bind(&Pitaya::Engine::Internal::Input::ProcessMouseButtonEvent, this, std::placeholders::_1));
	return true;
}
void Pitaya::Engine::Internal::Input::Release()
{
	Pitaya::Engine::Event::UnSubscribe(keyEventToken);
	Pitaya::Engine::Event::UnSubscribe(mouseButtonEventToken);
}