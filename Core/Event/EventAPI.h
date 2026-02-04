#pragma once

#include"../Enum/Enum.h"
#include"../Identifier/UID.h"

#include<functional>

namespace Pitaya::Core::Event
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
			struct CloseEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{

			};
			struct FramebufferResetSizeEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{
				FramebufferResetSizeEventArgs(int width, int height)
					:width(width), height(height) {}

				int width = 0;
				int height = 0;
			};
			struct DropFileEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{
				DropFileEventArgs(int count, const char** paths) 
					:count(count), paths(paths) {}

				int count = 0;
				const char** paths = nullptr;
			};
		}

		namespace Input
		{
			struct MouseCurrsorMoveEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{
				MouseCurrsorMoveEventArgs(double xPosition, double yPosition) 
					:xPosition(xPosition), yPosition(yPosition) {}

				double xPosition = 0;
				double yPosition = 0;
			};
			struct MouseScrollEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{
				MouseScrollEventArgs(double xOffset, double yOffset) 
					:xOffset(xOffset), yOffset(yOffset) {}

				double xOffset = 0;
				double yOffset = 0;
			};
			struct KeyDownEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{
				KeyDownEventArgs(int key, int scancode, int action, int mods) 
					:key(key), scancode(scancode), action(action), mods(mods) {}

				int key = 0;
				int scancode = 0;
				int action = 0;
				int mods = 0;
			};
			struct MouseButtonEventArgs : public Pitaya::Core::Event::Args::EventArgs
			{
				MouseButtonEventArgs(int button, int action, int mods) 
					:button(button), action(action), mods(mods) {}

				int button = 0;
				int action = 0;
				int mods = 0;
			};
		}
	}

	struct Event
	{
		Event(Pitaya::Core::Event::EventType type, const Pitaya::Core::Event::Args::EventArgs& args)
			:type(type), args(args) {}

		const Pitaya::Core::Event::EventType type;
		const Pitaya::Core::Event::Args::EventArgs& args;
	};

	struct EventToken
	{
		EventToken(UID uid = UID::Invalid, EventType type = EventType::Invalid)
			:uid(uid), type(type) {}

		bool operator==(const EventToken& other) const noexcept
		{ 
			return uid == other.uid;
		}

		const UID uid;
		const EventType type;
	};

	EventToken Subscribe(Pitaya::Core::Event::EventType type, std::function<void(const Pitaya::Core::Event::Event&)> function);
	bool UnSubscribe(const Pitaya::Core::Event::EventToken& eventToken);
	void Emit(const Pitaya::Core::Event::Event& event);
}

namespace std
{
	template <>
	struct hash<Pitaya::Core::Event::EventToken>
	{
		std::size_t operator()(const Pitaya::Core::Event::EventToken& eventToken) const noexcept
		{
			return std::hash<Pitaya::Core::UID>()(eventToken.uid);
		}
	};
}