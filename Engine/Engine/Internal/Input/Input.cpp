#include"input.h"

#include<GLFW/glfw3.h>

bool Pitaya::Engine::Internal::Input::Initialize()
{
	keyEventToken = Pitaya::Engine::Event::Subscribe(Pitaya::Engine::Event::EventType::Key, std::bind(&Pitaya::Engine::Internal::Input::ProcessKeyEvent, this, std::placeholders::_1));
	mouseButtonEventToken = Pitaya::Engine::Event::Subscribe(Pitaya::Engine::Event::EventType::Key, std::bind(&Pitaya::Engine::Internal::Input::ProcessMouseButtonEvent, this, std::placeholders::_1));

	//ЪѓБъ
	map[GLFW_MOUSE_BUTTON_1] = Pitaya::Engine::Input::KeyCode::Mouse0;
	map[GLFW_MOUSE_BUTTON_2] = Pitaya::Engine::Input::KeyCode::Mouse1;
	map[GLFW_MOUSE_BUTTON_3] = Pitaya::Engine::Input::KeyCode::Mouse2;
	map[GLFW_MOUSE_BUTTON_4] = Pitaya::Engine::Input::KeyCode::Mouse3;
	map[GLFW_MOUSE_BUTTON_5] = Pitaya::Engine::Input::KeyCode::Mouse4;

	//0-9
	map[GLFW_KEY_0] = Pitaya::Engine::Input::KeyCode::Key0;
	map[GLFW_KEY_1] = Pitaya::Engine::Input::KeyCode::Key1;
	map[GLFW_KEY_2] = Pitaya::Engine::Input::KeyCode::Key2;
	map[GLFW_KEY_3] = Pitaya::Engine::Input::KeyCode::Key3;
	map[GLFW_KEY_4] = Pitaya::Engine::Input::KeyCode::Key4;
	map[GLFW_KEY_5] = Pitaya::Engine::Input::KeyCode::Key5;
	map[GLFW_KEY_6] = Pitaya::Engine::Input::KeyCode::Key6;
	map[GLFW_KEY_7] = Pitaya::Engine::Input::KeyCode::Key7;
	map[GLFW_KEY_8] = Pitaya::Engine::Input::KeyCode::Key8;
	map[GLFW_KEY_9] = Pitaya::Engine::Input::KeyCode::Key9;

	//A-Z
	map[GLFW_KEY_A] = Pitaya::Engine::Input::KeyCode::A;
	map[GLFW_KEY_B] = Pitaya::Engine::Input::KeyCode::B;
	map[GLFW_KEY_C] = Pitaya::Engine::Input::KeyCode::C;
	map[GLFW_KEY_D] = Pitaya::Engine::Input::KeyCode::D;
	map[GLFW_KEY_E] = Pitaya::Engine::Input::KeyCode::E;
	map[GLFW_KEY_F] = Pitaya::Engine::Input::KeyCode::F;
	map[GLFW_KEY_G] = Pitaya::Engine::Input::KeyCode::G;
	map[GLFW_KEY_H] = Pitaya::Engine::Input::KeyCode::H;
	map[GLFW_KEY_I] = Pitaya::Engine::Input::KeyCode::I;
	map[GLFW_KEY_J] = Pitaya::Engine::Input::KeyCode::J;
	map[GLFW_KEY_K] = Pitaya::Engine::Input::KeyCode::K;
	map[GLFW_KEY_L] = Pitaya::Engine::Input::KeyCode::L;
	map[GLFW_KEY_M] = Pitaya::Engine::Input::KeyCode::M;
	map[GLFW_KEY_N] = Pitaya::Engine::Input::KeyCode::N;
	map[GLFW_KEY_O] = Pitaya::Engine::Input::KeyCode::O;
	map[GLFW_KEY_P] = Pitaya::Engine::Input::KeyCode::P;
	map[GLFW_KEY_Q] = Pitaya::Engine::Input::KeyCode::Q;
	map[GLFW_KEY_R] = Pitaya::Engine::Input::KeyCode::R;
	map[GLFW_KEY_S] = Pitaya::Engine::Input::KeyCode::S;
	map[GLFW_KEY_T] = Pitaya::Engine::Input::KeyCode::T;
	map[GLFW_KEY_U] = Pitaya::Engine::Input::KeyCode::U;
	map[GLFW_KEY_V] = Pitaya::Engine::Input::KeyCode::V;
	map[GLFW_KEY_W] = Pitaya::Engine::Input::KeyCode::W;
	map[GLFW_KEY_X] = Pitaya::Engine::Input::KeyCode::X;
	map[GLFW_KEY_Y] = Pitaya::Engine::Input::KeyCode::Y;
	map[GLFW_KEY_Z] = Pitaya::Engine::Input::KeyCode::Z;

	map[GLFW_KEY_J] = Pitaya::Engine::Input::KeyCode::Key9;
	return true;
}
void Pitaya::Engine::Internal::Input::Release()
{
	Pitaya::Engine::Event::UnSubscribe(keyEventToken);
	Pitaya::Engine::Event::UnSubscribe(mouseButtonEventToken);
}