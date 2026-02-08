#pragma once

#include"Enum/Enum.h"

#include<Engine/API/Event/Event.h>

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

	public:
		inline bool GetKeyDown(Pitaya::Engine::Input::KeyCode keyCode) const noexcept
		{ 
			return (keyCode != Pitaya::Engine::Input::KeyCode::Unknown) ? 
				currentFrameState[static_cast<size_t>(keyCode)] : false;
		}
		inline bool GetKeyPressed(Pitaya::Engine::Input::KeyCode keyCode) const noexcept
		{
			return (keyCode != Pitaya::Engine::Input::KeyCode::Unknown) ? 
				(currentFrameState[static_cast<size_t>(keyCode)] && !previousFrameState[static_cast<size_t>(keyCode)]) : false;
		}
		inline bool GetKeyReleased(Pitaya::Engine::Input::KeyCode keyCode) const noexcept
		{
			return (keyCode != Pitaya::Engine::Input::KeyCode::Unknown) ?
				(!currentFrameState[(size_t)keyCode] && previousFrameState[(size_t)keyCode]) : false;
		}

	private:
		inline void ProcessKeyEvent(const Pitaya::Engine::Event::Event& event)
		{
			if (event.type != Pitaya::Engine::Event::EventType::Key)
			{
				return;
			}

			const Pitaya::Engine::Event::Args::Input::KeyEventArgs& args = static_cast<const Pitaya::Engine::Event::Args::Input::KeyEventArgs&>(event.args);
			if (args.keycode != Pitaya::Engine::Input::KeyCode::Unknown)
			{
				hardwareState[static_cast<size_t>(args.keycode)] = (args.action == 1);
			}
		}
		inline void ProcessMouseButtonEvent(const Pitaya::Engine::Event::Event& event)
		{
			if (event.type != Pitaya::Engine::Event::EventType::MouseButton)
			{
				return;
			}

			const Pitaya::Engine::Event::Args::Input::MouseButtonEventArgs& args = static_cast<const Pitaya::Engine::Event::Args::Input::MouseButtonEventArgs&>(event.args);
			if (args.button != Pitaya::Engine::Input::KeyCode::Unknown)
			{
				hardwareState[static_cast<size_t>(args.button)] = (args.action == 1);
			}
		}

	private:
		Pitaya::Engine::Event::EventToken keyEventToken;
		Pitaya::Engine::Event::EventToken mouseButtonEventToken;

		std::vector<bool> currentFrameState = std::vector<bool>(static_cast<size_t>(Pitaya::Engine::Input::KeyCode::Unknown), false);
		std::vector<bool> previousFrameState = std::vector<bool>(static_cast<size_t>(Pitaya::Engine::Input::KeyCode::Unknown), false);
		std::vector<bool> hardwareState = std::vector<bool>(static_cast<size_t>(Pitaya::Engine::Input::KeyCode::Unknown), false);
	};
}