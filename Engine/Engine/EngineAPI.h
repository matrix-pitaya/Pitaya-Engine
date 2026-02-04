#pragma once

#include<Enum/Enum.h>
#include<Identifier/UID.h>

#include<string>
#include<functional>
#include<thread>

namespace Pitaya::Engine
{
	int Execute();
}

namespace Pitaya::Engine::Config
{
	size_t GetMaxFixupdataExecuteTimes() noexcept;
}

namespace Pitaya::Engine::Thread
{
	struct ThreadToken
	{
		explicit ThreadToken(std::thread::id id = std::thread::id())
			:id(id) {}

		bool operator==(const ThreadToken& other) const noexcept
		{
			return id == other.id;
		}

		std::thread::id id;

	public:
		struct Hash
		{
			size_t operator()(const ThreadToken& taskToken) const noexcept
			{
				return std::hash<std::thread::id>()(taskToken.id);
			}
		};
	};

	const std::string& GetThreadName(const std::thread::id& id) noexcept;
}

namespace Pitaya::Engine::Log
{
	void LogInfo(const std::string& message) noexcept;
	void LogDebug(const std::string& message) noexcept;
	void LogWarning(const std::string& message) noexcept;
	void LogError(const std::string& message) noexcept;
}

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
				KeyEventArgs(int key, int scancode, int action, int mods)
					:key(key), scancode(scancode), action(action), mods(mods) {}

				int key = 0;
				int scancode = 0;
				int action = 0;
				int mods = 0;
			};
			struct MouseButtonEventArgs : public Pitaya::Engine::Event::Args::EventArgs
			{
				MouseButtonEventArgs(int button, int action, int mods)
					:button(button), action(action), mods(mods) {}

				int button = 0;
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

		Pitaya::Core::UID uid;
		EventType type;
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

namespace Pitaya::Engine::Time
{
	extern const float& delta;
	extern const float& fixdelta;
	extern const float& unscaledDeltaTime;
	extern float& scale;

	float Seconds() noexcept;
	int64_t Milliseconds() noexcept;
}
