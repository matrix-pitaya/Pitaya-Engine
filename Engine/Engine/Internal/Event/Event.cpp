#include"Event.h"

#include<Utility/Utility.h>

bool Pitaya::Engine::Internal::Event::Initialize()
{
	return true;
}
void Pitaya::Engine::Internal::Event::Release()
{

}

Pitaya::Engine::Event::EventToken Pitaya::Engine::Internal::Event::Subscribe(Pitaya::Engine::Event::EventType type, std::function<void(const Pitaya::Engine::Event::Event&)> function) noexcept
{
	if (function == nullptr)
	{
		return Pitaya::Engine::Event::EventToken(Pitaya::Core::UID::Invalid, Pitaya::Engine::Event::EventType::Invalid);
	}

	Pitaya::Engine::Event::EventToken eventToken = Pitaya::Engine::Event::EventToken(Pitaya::Core::UID::Next(), type);
	eventMap[type][eventToken] = std::move(function);
	return eventToken;
}
bool Pitaya::Engine::Internal::Event::UnSubscribe(const Pitaya::Engine::Event::EventToken& eventToken) noexcept
{
	auto iterator = eventMap.find(eventToken.type);
	if (iterator == eventMap.end())
	{
		return false;
	}
	
	if (iterator->second.find(eventToken) == iterator->second.end())
	{
		return false;
	}

	iterator->second.erase(eventToken);
	if (iterator->second.empty())
	{
		eventMap.erase(eventToken.type);
	}
	return true;
}
void Pitaya::Engine::Internal::Event::Emit(const Pitaya::Engine::Event::Event& event) noexcept
{
	auto iterator = eventMap.find(event.type);
	if (iterator == eventMap.end())
	{
		return;
	}

	for (const auto& pair : iterator->second)
	{
		if (pair.second != nullptr)
		{
			pair.second(event);
		}
	}
}