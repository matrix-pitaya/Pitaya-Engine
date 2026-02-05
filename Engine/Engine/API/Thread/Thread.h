#pragma once

#include<thread>
#include<string>

namespace Pitaya::Engine::Internal
{
	class Thread;
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

	Pitaya::Engine::Internal::Thread* GetThreadModel() noexcept;
	const std::string& GetThreadName(std::thread::id id) noexcept;
}