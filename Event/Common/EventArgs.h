#pragma once

#include<Event/Common/EventType.h>
#include<Input/Common/KeyCode.h>

namespace Pitaya::Event
{
	struct CloseEventArgs
	{

	};
	struct FramebufferResetSizeEventArgs 
	{
		int width = 0;
		int height = 0;
	};
	struct DropFileEventArgs 
	{
		int count = 0;
		const char** paths = nullptr;
	};
	struct MouseCurrsorMoveEventArgs 
	{
		double xPosition = 0;
		double yPosition = 0;
	};
	struct MouseScrollEventArgs
	{
		double xOffset = 0;
		double yOffset = 0;
	};
	struct KeyEventArgs
	{
		Pitaya::Input::KeyCode code = Pitaya::Input::KeyCode::Unknown;
		int scancode = 0;
		int action = 0;
		int mods = 0;
	};
	struct MouseButtonEventArgs
	{
		Pitaya::Input::KeyCode button = Pitaya::Input::KeyCode::Unknown;
		int action = 0;
		int mods = 0;
	};

	union EventArgs
	{
		CloseEventArgs close = {};
		FramebufferResetSizeEventArgs framebufferSize;
		DropFileEventArgs dropFile;
		MouseCurrsorMoveEventArgs mouseCurrsor;
		MouseScrollEventArgs mouseScroll;
		KeyEventArgs key;
		MouseButtonEventArgs mouseButton;
	};
}
