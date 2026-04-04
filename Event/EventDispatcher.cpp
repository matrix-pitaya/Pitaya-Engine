#include<Event/EventDispatcher.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Event::EventDispatcher::Initialize()
{
	registry.Map.resize(static_cast<uint8_t>(Pitaya::Event::EventType::Invalid));
	return true;
}
void Pitaya::Event::EventDispatcher::Release()
{
	registry.Map.clear();
}
Pitaya::Event::EventToken Pitaya::Event::EventDispatcher::Subscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, const ::Pitaya::Event::Event&), void* listener) noexcept
{
	if (!OnCallBack || !listener || type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to subscibe a invalid event");
		return Pitaya::Event::EventToken(Pitaya::Event::EventType::Invalid);
	}

	Pitaya::Event::EventToken eventToken = Pitaya::Event::EventToken( type);
	registry.Map[static_cast<uint8_t>(type)][eventToken] = { OnCallBack ,listener };
	return eventToken;
}
bool Pitaya::Event::EventDispatcher::UnSubscribe(const Pitaya::Event::EventToken& eventToken) noexcept
{
	if (eventToken.type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to unsubscibe event from a incalid token");
		return false;
	}
	
	auto& map = registry.Map[static_cast<uint8_t>(eventToken.type)];
	auto iterator = map.find(eventToken);
	if (iterator == map.end())
	{
		Pitaya::Log::Warning("event token not in event map");
		return false;
	}

	map.erase(iterator);
	return true;
}
void Pitaya::Event::EventDispatcher::Emit(const Pitaya::Event::Event& event) noexcept
{
	if (event.type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to emit a invalid event");
		return;
	}

	auto& map = registry.Map[static_cast<uint8_t>(event.type)];
	for (const auto& pair : map)
	{
		pair.second.OnCallBack(pair.second.listener, event);
	}
}
