#pragma once

#include <cstdint>

namespace Pitaya::Core
{
	enum class Color : uint8_t
	{
		White,
		Black,
		Red,
		Green,
		Yellow,
		Blue
	};
}

namespace Pitaya::Core::Log
{
	enum class LogLevel : uint8_t
	{
		Info,
		Warning,
		Error,
		Debug
	};
}

namespace Pitaya::Core::Event
{
	enum class EventType : uint8_t
	{
		System = 0,

		//Window
		WindowClose,
		WindowFramebufferResetSize,
		DropFile,

		//Input
		MouseCurrsorMove,
		MouseScroll,
		MouseButton,
		KeyDown,

		Invalid = -1,
	};
}

namespace Pitaya::Engine
{
	enum class ComponentType : uint8_t
	{
		Transform = 0,
		RigidBody,

		Count
	};
}

namespace Pitaya::Engine::Input
{
	enum class KeyCode : uint8_t
	{
		W,
		A,
		S,
		D,

		Mouse0,
		Mouse1,
		Mouse2,

		ESC
	};
}

namespace Pitaya::Engine::Renderer
{
	enum class RenderQueue : uint8_t
	{
		A, B, C
	};

	enum class RendererAPI : uint8_t
	{
		OpenGL
	};
}

namespace Pitaya::Engine::Physics
{
	enum class PhysicsAPI : uint8_t
	{
		Bullet
	};

	enum class RigidBodyType : uint8_t
	{
		Static,
		Dynamic,
		Kinematic
	};
}