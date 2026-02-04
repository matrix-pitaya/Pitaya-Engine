#include"input.h"

#include"../../Engine.h"

void Pitaya::Engine::Internal::Input::Listen()
{
	for (auto& pair : keyMap)
	{
		pair.second = Pitaya::Engine::Engine::Instance().GetKeyDownByWindow(pair.first);
	}
}
bool Pitaya::Engine::Internal::Input::GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode)
{
	return keyMap.at(keyCode);
}