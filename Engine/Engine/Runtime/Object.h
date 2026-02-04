#pragma once

#include"Identifier/UID.h"

namespace Pitaya::Engine
{
	class Object
	{
	public:
		Object() = default;
		~Object() = default;
		Object(const Object&) = default;
		Object& operator=(const Object&) = default;
		Object(Object&&) = default;
		Object& operator=(Object&&) = default;

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