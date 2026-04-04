#pragma once

#include<Engine/API/def.h>

namespace Pitaya::Engine
{
	ENGINE_API int ENGINE_CALL Execute(int, char**);
	ENGINE_API void ENGINE_CALL Terminate();


	//TODO 模拟dll加载，将Editor设置为导出dll后移除
	ENGINE_API void ENGINE_CALL EditorMountHook();
}
