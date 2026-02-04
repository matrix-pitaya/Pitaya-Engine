#include"Event.h"
#include"EventAPI.h"

#include"../Utility/Utility.h"

Pitaya::Core::Event::EventToken Pitaya::Core::Event::EventManager::Subscribe(Pitaya::Core::Event::EventType type, std::function<void(const Pitaya::Core::Event::Event&)> function)
{
	if (function == nullptr)
	{
		return EventToken(UID::Invalid, Pitaya::Core::Event::EventType::Invalid);
	}

	EventToken eventToken = EventToken(Pitaya::Core::UID::Next(), type);
	eventMap[type][eventToken] = std::move(function);
	return eventToken;
}
bool Pitaya::Core::Event::EventManager::UnSubscribe(const Pitaya::Core::Event::EventToken& eventToken)
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
void Pitaya::Core::Event::EventManager::Emit(const Pitaya::Core::Event::Event& event)
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