#pragma once

#include<Enum/Enum.h>
#include<Identifier/UID.h>

namespace Pitaya::Engine::Event
{
	namespace Args
	{
		struct EventArgs
		{
			EventArgs() = default;
			virtual ~EventArgs() = default;
		};

		namespace Window
		{
			struct CloseEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{

			};
			struct FramebufferResetSizeEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				FramebufferResetSizeEventArgs(int width, int height)
					:width(width), height(height) {
				}

				int width = 0;
				int height = 0;
			};
			struct DropFileEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				DropFileEventArgs(int count, const char** paths)
					:count(count), paths(paths) {
				}

				int count = 0;
				const char** paths = nullptr;
			};
		}

		namespace Input
		{
			struct MouseCurrsorMoveEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				MouseCurrsorMoveEventArgs(double xPosition, double yPosition)
					:xPosition(xPosition), yPosition(yPosition) {
				}

				double xPosition = 0;
				double yPosition = 0;
			};
			struct MouseScrollEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				MouseScrollEventArgs(double xOffset, double yOffset)
					:xOffset(xOffset), yOffset(yOffset) {
				}

				double xOffset = 0;
				double yOffset = 0;
			};
			struct KeyEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				KeyEventArgs(Pitaya::Engine::Input::KeyCode keycode, int scancode, int action, int mods)
					:keycode(keycode), scancode(scancode), action(action), mods(mods) {}

				Pitaya::Engine::Input::KeyCode keycode = Pitaya::Engine::Input::KeyCode::Unknown;
				int scancode = 0;
				int action = 0;
				int mods = 0;
			};
			struct MouseButtonEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				MouseButtonEventArgs(Pitaya::Engine::Input::KeyCode button, bool action, int mods)
					:button(button), action(action), mods(mods) {}

				Pitaya::Engine::Input::KeyCode button = Pitaya::Engine::Input::KeyCode::Unknown;
				int action = 0;
				int mods = 0;
			};
		}
	}

	struct EventToken
	{
		EventToken(Pitaya::Core::UID uid = Pitaya::Core::UID::Invalid, EventType type = EventType::Invalid)
			:uid(uid), type(type) {}
		EventToken(const EventToken&) = default;
		EventToken& operator=(const EventToken&) = default;

		bool operator==(const EventToken& other) const noexcept
		{
			return uid == other.uid;
		}

		Pitaya::Core::UID uid = Pitaya::Core::UID::Invalid;
		EventType type = EventType::Invalid;
	};
	struct Event
	{
		Event(Pitaya::Engine::Event::EventType type, const Pitaya::Engine::Event::Args::EventArgs& args)
			:type(type), args(args) {
		}

		const Pitaya::Engine::Event::EventType type;
		const Pitaya::Engine::Event::Args::EventArgs& args;
	};

	Pitaya::Engine::Event::EventToken Subscribe(::Pitaya::Engine::Event::EventType type, std::function<void(const ::Pitaya::Engine::Event::Event&)> function) noexcept;
	bool UnSubscribe(const ::Pitaya::Engine::Event::EventToken& eventToken) noexcept;
	void Emit(const ::Pitaya::Engine::Event::Event& event) noexcept;
}
namespace std
{
	template <>
	struct hash<Pitaya::Engine::Event::EventToken>
	{
		std::size_t operator()(const Pitaya::Engine::Event::EventToken& eventToken) const noexcept
		{
			return std::hash<Pitaya::Core::UID>()(eventToken.uid);
		}
	};
}