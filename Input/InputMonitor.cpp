#include<Input/InputMonitor.h>
#include<Event/Common/FuncTable.h>

bool Pitaya::Input::InputMonitor::Initialize()
{
	keyToken = Pitaya::Event::Subscribe(Pitaya::Event::EventType::Key, &Pitaya::Input::InputMonitor::OnKey, this);
	mouseButtonToken = Pitaya::Event::Subscribe(Pitaya::Event::EventType::MouseButton, &Pitaya::Input::InputMonitor::OnMouseButton, this);
	return true;
}
void Pitaya::Input::InputMonitor::Release()
{
	Pitaya::Event::UnSubscribe(keyToken);
	Pitaya::Event::UnSubscribe(mouseButtonToken);
}
