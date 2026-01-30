#include"Event.h"
#include"EventAPI.h"

Pitaya::Core::Event::EventToken Pitaya::Core::Event::Subscribe(Pitaya::Core::Event::EventType type, std::function<void(const Pitaya::Core::Event::Event&)> function)
{
	return EventManager::Instance.Subscribe(type, std::move(function));
}
bool Pitaya::Core::Event::UnSubscribe(const Pitaya::Core::Event::EventToken& eventToken)
{
	return EventManager::Instance.UnSubscribe(eventToken);
}
void Pitaya::Core::Event::Emit(const Pitaya::Core::Event::Event& event)
{
	return EventManager::Instance.Emit(event);
}
