#pragma once

namespace Pitaya::Engine
{
	template<typename Func>
	struct Hook
	{
		Func* Func = nullptr;
		const char* Name = nullptr;
	};
}