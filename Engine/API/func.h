#pragma once

#include<Engine/API/def.h>

namespace Pitaya::Engine
{
	extern "C" ENGINE_API int ENGINE_CALL Execute(int argc, char** argv);
	extern "C" ENGINE_API void ENGINE_CALL Terminate();
}
