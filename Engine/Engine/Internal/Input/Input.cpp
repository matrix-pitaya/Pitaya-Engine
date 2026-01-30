#include"input.h"

#include"../../Engine.h"

bool Pitaya::Engine::Internal::Input::Initialize()
{
	if (isInitialized)
	{
		return false;
	}

	keyMap.emplace(Pitaya::Engine::Input::KeyCode::W, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::A, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::S, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::D, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::Mouse0, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::Mouse1, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::Mouse2, false);
	keyMap.emplace(Pitaya::Engine::Input::KeyCode::ESC, false);

	isInitialized = true;
	return true;
}
void Pitaya::Engine::Internal::Input::Release()
{
	if (isReleased)
	{
		return;
	}

	isReleased = true;
}
void Pitaya::Engine::Internal::Input::Listen()
{
	for (auto& pair : keyMap)
	{
		pair.second = Engine::Instance.GetKeyDown(pair.first);
	}
}
bool Pitaya::Engine::Internal::Input::GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode)
{
	return keyMap.at(keyCode);
}