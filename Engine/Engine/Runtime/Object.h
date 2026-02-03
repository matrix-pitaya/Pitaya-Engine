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
		inline Pitaya::Core::UID SetUID(Pitaya::Core::UID uid) noexcept
		{
			this->uid = uid;
		}

	protected:
		Pitaya::Core::UID uid = Pitaya::Core::UID::Next();
	};
}