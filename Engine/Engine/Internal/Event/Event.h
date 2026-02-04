#pragma once

#include<Enum/Enum.h>
#include<Identifier/UID.h>

#include<functional>
#include<unordered_map>

#include "../../EngineAPI.h"

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Internal
{
	class Event
	{
		friend class Pitaya::Engine::Engine;
	private:
		Event() = default;
		~Event() = default;

	public:
		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;
		Event(Event&&) = delete;
		Event& operator=(Event&&) = delete;

	private:
		bool Initialize();
		void Release();

	private:
		Pitaya::Engine::Event::EventToken Subscribe(::Pitaya::Engine::Event::EventType type, std::function<void(const ::Pitaya::Engine::Event::Event&)> function) noexcept;
		bool UnSubscribe(const ::Pitaya::Engine::Event::EventToken& eventToken) noexcept;
		void Emit(const ::Pitaya::Engine::Event::Event& event) noexcept;

	private:
		std::unordered_map<::Pitaya::Engine::Event::EventType,
			std::unordered_map<::Pitaya::Engine::Event::EventToken, std::function<void(const ::Pitaya::Engine::Event::Event&)>>> eventMap;
	};
}