#pragma once

#include<cstdint>

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

namespace Pitaya::Engine
{
	enum class ComponentType : uint8_t
	{
		Transform = 0,
		RigidBody,

		Unknown
	};
}

namespace Pitaya::Engine::Log
{
	enum class LogLevel : uint8_t
	{
		Info,
		Warning,
		Error,
		Debug
	};
}

namespace Pitaya::Engine::Event
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
		Key,

		Invalid,
	};
}

namespace Pitaya::Engine::Input
{
	enum class KeyCode : uint8_t
	{
		Mouse0,		//鼠标左键
		Mouse1,		//鼠标右键
		Mouse2,		//鼠标中建
		Mouse3,		//鼠标侧键1
		Mouse4,		//鼠标侧键2
		
		//数字键0-9
		Key0,
		Key1,
		Key2,
		Key3,
		Key4,
		Key5,
		Key6,
		Key7,
		Key8,
		Key9,

		//A-Z
		A,B,C,D,E,F,J,H,I,G,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,

		Space,			//空格
		Semicolon,		//分号
		Equal,			//等于号
		
		BracketLeft,	//左中括号
		BracketRight,	//右中括号
		
		Comma,			//逗号
		Escape,			//ESC
		Enter,			//回车
		Tab,			//Tab
		Backspace,		//退格
		Insert,			//插入
		Delete,			//删除键

		Right,			//右
		Left,			//左
		Down,			//下
		UP,				//上

		PageUp,			//分页上
		PageDown,		//分页下

		Home,
		End,

		CapsLock,		//大写锁定

		//F1-F12
		F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,

		LeftShit,
		LeftControl,
		LeftAlt,
		LeftSuper,

		RightShit,
		RightControl,
		RightAlt,
		RightSuper,

		Unknown
	};
}

namespace Pitaya::Engine::Window
{
	enum class Platform : uint8_t
	{
		GLFW = 0,

		Unknown
	};
}

namespace Pitaya::Engine::Renderer
{
	enum class API : uint8_t
	{
		OpenGL = 0,

		Unknown
	};

	enum class RenderQueue : uint8_t
	{
		A, B, C
	};
}

namespace Pitaya::Engine::Physics
{
	enum class API : uint8_t
	{
		Bullet = 0,

		Unknown
	};

	enum class RigidBodyType : uint8_t
	{
		Static,
		Dynamic,
		Kinematic
	};
}