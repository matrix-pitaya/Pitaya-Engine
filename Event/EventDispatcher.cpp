#include<Event/EventDispatcher.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Event::EventDispatcher::Initialize()
{
	return true;
}
void Pitaya::Event::EventDispatcher::Release()
{
	registry.Clear();
}
Pitaya::Event::EventToken Pitaya::Event::EventDispatcher::Subscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, const ::Pitaya::Event::Event&), void* listener) noexcept
{
	if (!OnCallBack || !listener || type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to subscibe a invalid event");
		return Pitaya::Event::EventToken(Pitaya::Event::EventType::Invalid);
	}

	Pitaya::Event::EventToken eventToken { type };
	if (!registry.Emplace(type, eventToken, { OnCallBack ,listener }))
	{
		Pitaya::Log::Error("Event Registry Emplace Fail");
		return Pitaya::Event::EventToken(Pitaya::Event::EventType::Invalid);
	}
	return eventToken;
}
bool Pitaya::Event::EventDispatcher::UnSubscribe(const Pitaya::Event::EventToken& eventToken) noexcept
{
	if (eventToken.type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to unsubscibe event from a incalid token");
		return false;
	}
	
	if (!registry.Contains(eventToken))
	{
		Pitaya::Log::Warning("event token not in event map");
		return false;
	}

	if (!registry.Erase(eventToken))
	{
		Pitaya::Log::Error("event registry erase fail");
		return false;
	}

	return true;
}
void Pitaya::Event::EventDispatcher::Emit(const Pitaya::Event::Event& event) noexcept
{
	if (event.type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to emit a invalid event");
		return;
	}

	registry.ForEach(event.type, 
		[&event](EventToken, CallBack callback) { callback.OnCallBack(callback.listener, event); });
}
