#pragma once

#include<Event/Common/EventType.h>
#include<Input/Common/KeyCode.h>

namespace Pitaya::Event
{
	struct EventArgs
	{
		EventArgs() = default;
		virtual ~EventArgs() = default;
	};
	struct CloseEventArgs final : public EventArgs
	{

	};
	struct FramebufferResetSizeEventArgs final : public EventArgs
	{
		FramebufferResetSizeEventArgs(int width, int height)
			:width(width), height(height) { }

		int width = 0;
		int height = 0;
	};
	struct DropFileEventArgs final : public EventArgs
	{
		DropFileEventArgs(int count, const char** paths)
			:count(count), paths(paths) { }

		int count = 0;
		const char** paths = nullptr;
	};
	struct MouseCurrsorMoveEventArgs final : public EventArgs
	{
		MouseCurrsorMoveEventArgs(double xPosition, double yPosition)
			:xPosition(xPosition), yPosition(yPosition) { }

		double xPosition = 0;
		double yPosition = 0;
	};
	struct MouseScrollEventArgs final : public EventArgs
	{
		MouseScrollEventArgs(double xOffset, double yOffset)
			:xOffset(xOffset), yOffset(yOffset) { }

		double xOffset = 0;
		double yOffset = 0;
	};
	struct KeyEventArgs final : public EventArgs
	{
		KeyEventArgs(Pitaya::Input::KeyCode keycode, int scancode, int action, int mods)
			:keycode(keycode), scancode(scancode), action(action), mods(mods) { }

		Pitaya::Input::KeyCode keycode = Pitaya::Input::KeyCode::Unknown;
		int scancode = 0;
		int action = 0;
		int mods = 0;
	};
	struct MouseButtonEventArgs final : public EventArgs
	{
		MouseButtonEventArgs(Pitaya::Input::KeyCode button, int action, int mods)
			:button(button), action(action), mods(mods) { }

		Pitaya::Input::KeyCode button = Pitaya::Input::KeyCode::Unknown;
		int action = 0;
		int mods = 0;
	};
}
