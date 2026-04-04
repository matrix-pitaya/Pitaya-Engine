#pragma once

#include<Engine/API/def.h>

namespace Pitaya::Engine
{
	extern "C" ENGINE_API int ENGINE_CALL Execute(int, char**);
	extern "C" ENGINE_API void ENGINE_CALL Terminate();




	//TODO 模拟dll加载，将Editor设置为导出dll后移除
	extern "C" ENGINE_API void ENGINE_CALL EditorMountHook();
}
