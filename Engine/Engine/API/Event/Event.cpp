#include<Engine/API/Event/Event.h>

#include<Engine/Engine.h>
#include<Engine/Internal/Event/Event.h>

Pitaya::Engine::Event::EventToken Pitaya::Engine::Event::Subscribe(::Pitaya::Engine::Event::EventType type, std::function<void(const ::Pitaya::Engine::Event::Event&)> function) noexcept
{
	return Pitaya::Engine::Engine::Instance().GetEventModel()->Subscribe(type, std::move(function));
}
bool Pitaya::Engine::Event::UnSubscribe(const ::Pitaya::Engine::Event::EventToken& eventToken) noexcept
{
	return Pitaya::Engine::Engine::Instance().GetEventModel()->UnSubscribe(eventToken);
}
void Pitaya::Engine::Event::Emit(const ::Pitaya::Engine::Event::Event& event) noexcept
{
	Pitaya::Engine::Engine::Instance().GetEventModel()->Emit(event);
}