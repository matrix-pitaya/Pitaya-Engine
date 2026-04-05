#pragma once

#include<Engine/API/def.h>

namespace Pitaya::Engine
{
	extern "C" ENGINE_API int ENGINE_CALL Execute(int, char**);
	extern "C" ENGINE_API void ENGINE_CALL Terminate();
}
