#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Context/Common/Module.h>

#include<Input/Common/KeyCode.h>
#include<Event/Common/Event.h>

namespace Pitaya::Input
{
	class InputMonitor
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<InputMonitor>;
		private:
			static InputMonitor* Create()
			{
				return PITAYA_NEW(InputMonitor);
			}
			static void Destroy(InputMonitor* inputMonitor)
			{
				PITAYA_DELETE(inputMonitor);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<InputMonitor>;
		private:
			static bool Initialize(InputMonitor* inputMonitor)
			{
				return inputMonitor->Initialize();
			}
			static void Release(InputMonitor* inputMonitor)
			{
				inputMonitor->Release();
			}
		};

	private:
		InputMonitor() = default;
		~InputMonitor() = default;

	public:
		InputMonitor(const InputMonitor&) = delete;
		InputMonitor& operator=(const InputMonitor&) = delete;
		InputMonitor(InputMonitor&&) = delete;
		InputMonitor& operator=(InputMonitor&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		inline void PrepareNewFrame(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
		{
			std::memcpy(previousFrameState, currentFrameState, sizeof(previousFrameState));
		}
		inline void UpdateSnapshots(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
		{
			std::memcpy(currentFrameState, hardwareState, sizeof(currentFrameState));
		}

	public:
		inline bool GetKeyDown(Pitaya::Input::KeyCode keyCode) const noexcept
		{ 
			return (keyCode != Pitaya::Input::KeyCode::Unknown) ?
				currentFrameState[static_cast<size_t>(keyCode)] : false;
		}
		inline bool GetKeyPressed(Pitaya::Input::KeyCode keyCode) const noexcept
		{
			return (keyCode != Pitaya::Input::KeyCode::Unknown) ?
				(currentFrameState[static_cast<size_t>(keyCode)] && !previousFrameState[static_cast<size_t>(keyCode)]) : false;
		}
		inline bool GetKeyReleased(Pitaya::Input::KeyCode keyCode) const noexcept
		{
			return (keyCode != Pitaya::Input::KeyCode::Unknown) ?
				(!currentFrameState[(size_t)keyCode] && previousFrameState[(size_t)keyCode]) : false;
		}

	private:
		inline void OnKey(Pitaya::Event::Event event)
		{
			if (event.args.key.code != Pitaya::Input::KeyCode::Unknown)
			{
				hardwareState[static_cast<size_t>(event.args.key.code)] = (event.args.key.action != 0);
			}
		}
		inline void OnMouseButton(Pitaya::Event::Event event)
		{
			if (event.args.mouseButton.button != Pitaya::Input::KeyCode::Unknown)
			{
				hardwareState[static_cast<size_t>(event.args.mouseButton.button)] = (event.args.mouseButton.action != 0);
			}
		}

	private:
		inline static void OnKey(void* listener, Pitaya::Event::Event event)
		{
			static_cast<Pitaya::Input::InputMonitor*>(listener)->OnKey(event);
		}
		inline static void OnMouseButton(void* listener, Pitaya::Event::Event event)
		{
			static_cast<Pitaya::Input::InputMonitor*>(listener)->OnMouseButton(event);
		}

	private:
		bool currentFrameState[static_cast<size_t>(Pitaya::Input::KeyCode::Unknown)] = {};
		bool previousFrameState[static_cast<size_t>(Pitaya::Input::KeyCode::Unknown)] = {};
		bool hardwareState[static_cast<size_t>(Pitaya::Input::KeyCode::Unknown)] = {};

		Pitaya::Event::EventToken keyToken;
		Pitaya::Event::EventToken mouseButtonToken;
	};
}
