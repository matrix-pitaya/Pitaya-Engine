#pragma once

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
		friend class Pitaya::Engine::Engine;
	private:
		Input()
		{
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::W, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::A, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::S, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::D, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::Mouse0, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::Mouse1, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::Mouse2, false);
			keyMap.emplace(Pitaya::Engine::Input::KeyCode::ESC, false);
		}
		~Input() = default;

	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		Input(Input&&) = delete;
		Input& operator=(Input&&) = delete;

	private:
		void Listen();
		bool GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode);

	private:
		std::unordered_map<Pitaya::Engine::Input::KeyCode, bool> keyMap;
	};
}