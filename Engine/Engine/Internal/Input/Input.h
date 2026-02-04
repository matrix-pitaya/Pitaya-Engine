#pragma once

#include"Enum/Enum.h"
#include"../../EngineAPI.h"

#include<vector>

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
		Input() = default;
		~Input() = default;

	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		Input(Input&&) = delete;
		Input& operator=(Input&&) = delete;

	private:
		bool Initialize();
		void Release();

	private:
		inline void PrepareNewFrame() noexcept
		{
			previousFrameState = currentFrameState;
		}
		inline void UpdateSnapshots() noexcept
		{
			currentFrameState = hardwareState;
		}
		inline bool IsKeyDown(Pitaya::Engine::Input::KeyCode keyCode) const noexcept
		{ 
			return (keyCode != Pitaya::Engine::Input::KeyCode::Unknown) ? 
				currentFrameState[static_cast<size_t>(keyCode)] : false;
		}
		inline bool IsKeyPressed(Pitaya::Engine::Input::KeyCode keyCode) const noexcept
		{
			return (keyCode != Pitaya::Engine::Input::KeyCode::Unknown) ? 
				(currentFrameState[static_cast<size_t>(keyCode)] && !previousFrameState[static_cast<size_t>(keyCode)]) : false;
		}
		inline bool IsKeyReleased(Pitaya::Engine::Input::KeyCode keyCode) const noexcept
		{
			return (keyCode != Pitaya::Engine::Input::KeyCode::Unknown) ? (!currentFrameState[(size_t)keyCode] && previousFrameState[(size_t)keyCode]) : false;
		}
		inline void ProcessKeyEvent(const Pitaya::Engine::Event::Event& event)
		{
			if (event.type != Pitaya::Engine::Event::EventType::Key)
			{
				return;
			}

			const Pitaya::Engine::Event::Args::Input::KeyEventArgs& args = static_cast<const Pitaya::Engine::Event::Args::Input::KeyEventArgs&>(event.args);
			Pitaya::Engine::Input::KeyCode keyCode = IntToKeyCode(args.key);
			if (keyCode != Pitaya::Engine::Input::KeyCode::Unknown)
			{
				hardwareState[static_cast<size_t>(keyCode)] = args.action == 1;
			}
		}
		inline void ProcessMouseButtonEvent(const Pitaya::Engine::Event::Event& event)
		{
			if (event.type != Pitaya::Engine::Event::EventType::MouseButton)
			{
				return;
			}

			const Pitaya::Engine::Event::Args::Input::MouseButtonEventArgs& args = static_cast<const Pitaya::Engine::Event::Args::Input::MouseButtonEventArgs&>(event.args);
			Pitaya::Engine::Input::KeyCode keyCode = IntToKeyCode(args.button);
			if (keyCode != Pitaya::Engine::Input::KeyCode::Unknown)
			{
				hardwareState[static_cast<size_t>(keyCode)] = args.action == 1;
			}
		}
		inline Pitaya::Engine::Input::KeyCode IntToKeyCode(int key) const noexcept
		{
			return (key < 0 || key >= 512) ? Pitaya::Engine::Input::KeyCode::Unknown : map[key];
		}

	private:
		Pitaya::Engine::Event::EventToken keyEventToken;
		Pitaya::Engine::Event::EventToken mouseButtonEventToken;

		std::vector<bool> currentFrameState = std::vector<bool>(static_cast<size_t>(Pitaya::Engine::Input::KeyCode::Unknown), false);
		std::vector<bool> previousFrameState = std::vector<bool>(static_cast<size_t>(Pitaya::Engine::Input::KeyCode::Unknown), false);
		std::vector<bool> hardwareState = std::vector<bool>(static_cast<size_t>(Pitaya::Engine::Input::KeyCode::Unknown), false);

		std::vector<Pitaya::Engine::Input::KeyCode> map = std::vector<Pitaya::Engine::Input::KeyCode>(512, Pitaya::Engine::Input::KeyCode::Unknown);
	};
}