#pragma once

#include<Event/Common/EventType.h>

#include<cstdint>
#include<atomic>

namespace Pitaya::Event
{
	struct EventToken
	{
		EventToken(EventType type = EventType::Invalid)
			: type(type) {}
		EventToken(const EventToken&) = default;
		EventToken& operator=(const EventToken&) = default;

		bool operator==(const EventToken& other) const noexcept
		{
			return id == other.id;
		}

	public:
		uint32_t id = Next();
		EventType type = EventType::Invalid;

	private:
		inline static uint32_t Next() noexcept
		{
			static std::atomic<uint32_t> id = 1;
			return id.fetch_add(1, std::memory_order_relaxed);
		}
	};
}

namespace std
{
	template <>
	struct hash<Pitaya::Event::EventToken>
	{
		std::size_t operator()(const Pitaya::Event::EventToken& eventToken) const noexcept
		{
			return std::hash<uint64_t>()(eventToken.id);
		}
	};
}
