#pragma once

#include"Define/Define.h"
#include"Enum/Enum.h"

#include<unordered_map>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Internal
{
	class Input
	{
		DECLARE_ENGINE_SUBSYSTEM(Input)

	private:
		void Listen();
		bool GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode);

	private:
		std::unordered_map<Pitaya::Engine::Input::KeyCode, bool> keyMap;
	};
}