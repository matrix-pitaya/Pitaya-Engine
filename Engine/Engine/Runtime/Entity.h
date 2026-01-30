#pragma once

#include"Define/Define.h"
#include"Utility/Utility.h"

namespace Pitaya::Engine
{
	class Entity
	{
		Default_Constructor_VirtualDestructor_Move_Copy(Entity)

	protected:
		Pitaya::Core::UID uid = Pitaya::Core::UID::Next();
	};
}