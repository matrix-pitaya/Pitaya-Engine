#pragma once

#include"Define/Define.h"
#include"Identifier/UID.h"

namespace Pitaya::Engine
{
	class Object
	{
		DEFAULT_CONSTRUCTOR_VIRTUALDESTRUCTOR_MOVE_COPY(Object)

	public:
		inline Pitaya::Core::UID GetUID() const noexcept
		{
			return uid;
		}

	protected:
		Pitaya::Core::UID uid = Pitaya::Core::UID::Next();
	};
}