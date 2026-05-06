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
Pitaya::Event::EventToken Pitaya::Event::EventDispatcher::Subscribe(Pitaya::Event::EventType type, void (*OnCallBack)(void*, Pitaya::Event::Event), void* listener) noexcept
{
	if (!OnCallBack || !listener || type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to subscibe a invalid event");
		return { Pitaya::Event::EventType::Invalid , Pitaya::Core::SlotMap<EventCallBack>::Handle::Invalid };
	}

	auto handle = registry.Emplace(type, { OnCallBack ,listener });
	if (!handle)
	{
		Pitaya::Log::Error("Event Registry Emplace Fail");
		return { Pitaya::Event::EventType::Invalid , Pitaya::Core::SlotMap<EventCallBack>::Handle::Invalid };
	}
	return { type, handle };
}
bool Pitaya::Event::EventDispatcher::UnSubscribe(Pitaya::Event::EventToken eventToken) noexcept
{
	if (eventToken.Type() == Pitaya::Event::EventType::Invalid || !eventToken.Handle())
	{
		Pitaya::Log::Warning("try to unsubscibe event from a incalid token");
		return false;
	}

	if (!registry.Contains(eventToken.Type(), eventToken.Handle()))
	{
		Pitaya::Log::Warning("event token not in event map");
		return false;
	}

	if (!registry.Erase(eventToken.Type(), eventToken.Handle()))
	{
		Pitaya::Log::Error("event registry erase fail");
		return false;
	}

	return true;
}
void Pitaya::Event::EventDispatcher::Emit(Pitaya::Event::Event event) noexcept
{
	if (event.type == Pitaya::Event::EventType::Invalid)
	{
		Pitaya::Log::Warning("try to emit a invalid event");
		return;
	}

	for (auto [handle, callback] : registry.Each(event.type))
	{
		callback.OnCallBack(callback.listener, event);
	}
}
