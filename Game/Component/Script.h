#pragma once

#include<vector>
#include<string>

namespace Pitaya::Game
{
	struct Script
	{
        struct ScriptInstance 
        {
            std::string_view ClassName = "MonoBehaviour";     // 脚本名
            void* MonoObject = nullptr;                       // 指向 C# 里 new 出来的实际对象
        };

    public:
        inline const std::vector<ScriptInstance>& GetScriptInstance() const noexcept
        {
            return scriptInstances;
		}

    private:
        std::vector<ScriptInstance> scriptInstances = { { "MonoBehaviour", nullptr} };  //TODO 测试UI用
	};
}