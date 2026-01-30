#pragma once

#include"Enum/Enum.h"

#include"Object.h"

namespace Pitaya::Engine
{
	class Component : public Object
	{
		Default_Constructor_VirtualDestructor_Move_Copy(Component)
	};
}